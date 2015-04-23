//
//  Copyright (c) 2014 Moodstocks. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

/** Enumeration for scan result types.
 */
typedef NS_ENUM(int, MSResultType) {
    /** Undefined result type. */
    MSResultTypeNone          = 0,
    /** EAN8 linear barcode. */
    MSResultTypeEAN8          = 1 << 0,
    /** EAN13 linear barcode. */
    MSResultTypeEAN13         = 1 << 1,
    /** QR Code 2D barcode */
    MSResultTypeQRCode        = 1 << 2,
    /** Datamatrix 2D barcode */
    MSResultTypeDatamatrix    = 1 << 3,
    /** Image match */
    MSResultTypeImage         = 1 << 31
};

/** Enumeration for scan result origins (client-side vs. server-side).
 */
typedef NS_ENUM(int, MSResultOrigin) {
    /** Undefined scan origin. */
    MSResultOriginNone        = 0,
    /** Result found on client-side. */
    MSResultOriginClient      = 1 << 0,
    /** Result found on server-side. */
    MSResultOriginServer      = 1 << 1,
};

/** Enumeration for scan result extra information.
 */
typedef NS_ENUM(int, MSResultExtra) {
    /** Require no geometrical information. */
    MSResultExtraNone         = 0,
    /** Require the recognized image or barcode corners. */
    MSResultExtraCorners      = 1 << 0,
    /** Require the homography linking the query frame and the recognized reference image. */
    MSResultExtraHomography   = 1 << 1,
    /** Require the dimensions of the recognized reference image. */
    MSResultExtraDimensions   = 1 << 2,
};

/** Convenient flag that combines all barcodes types.
 */
extern const int kMSResultAllBarcodes;

/** The `MSResult` class represents a successful scan result with extra information.
 *
 * In practice you never need to create and initialize a `MSResult` by yourself. Instead
 * you use one that is returned by a MSScanner object or a scanner session (MSAutoScannerSession
 * or MSManualScannerSession).
 *
 * Some extra information such as corners or homography are not systematically available:
 * please refer to their documentation below for more details.
 */
@interface MSResult : NSObject

/** The scan result type.
 */
@property (nonatomic, readonly) MSResultType type;

/** The scan result origin.
 */
@property (nonatomic, readonly) MSResultOrigin origin;

/** The data object containing the scan result raw data.
 */
@property (nonatomic, readonly, strong) NSData *data;

/** The UTF-8 encoded string representing the scan result.
 */
@property (nonatomic, readonly, strong) NSString *string;

/** The recognized image or barcode corners.
 *
 * If not `nil` the corners are returned as a `CGPoint[4]` array which means you can
 * get them as follow:
 *
 * `CGPoint corners[4]; [result.corners getValue:&corners];`
 *
 * In order to make abstraction of the query frame dimensions, the returned coordinates are in the
 * [-1, 1] range if the point lies inside the query frame, i.e. a point with coordinates
 * `(-1.0,-1.0)` is located at the upper-left corners of the screen in its current UI orientation,
 * `(1.0,1.0)` at the bottom-right, `(0.0,0.0)` the center, etc... Note that they are not clamped
 * within the query frame boundaries and could thus take values outside the [-1, 1] range.
 *
 * @return Returns the `NSValue` containing the recognized image or barcode corners, or `nil` if the
 * corners are not available or if they have not been properly requested with the `MSResultExtraCorners`
 * flag.
 *
 * @warning **Important:** the corners are only available for results with `MSResultOriginClient`
 * origin.
 */
@property (nonatomic, readonly, strong) NSValue *corners;

/** The homography linking the query frame and the recognized reference image.
 *
 * If not `nil` the homography is returned as a `float[9]` array (= 3x3 matrix in row major) which
 * means you can get it as follow:
 *
 * `float homography[9]; [result.homography getValue:&homography];`
 *
 * In order to make abstraction of the frame and reference image dimensions, this homography is
 * computed with the assumption that both the frame and the reference image coordinates system
 * are in the [-1, 1] range.
 *
 * This homography can be used to project reference image points into the query frame domain by
 * using homogeneous coordinates, i.e.:
 *
 *     P' = H x P
 *     with: P = [x, y, 1]t and P' = [u, v, w]t
 *
 * @return Returns the `NSValue` containing the homography, or `nil` if the homography is not available
 * or has not been properly requested with the `MSResultExtraHomography` flag.
 *
 * @warning **Important:** the homography is only available for results of type `MSResultTypeImage`
 * with `MSResultOriginClient` origin.
 */
@property (nonatomic, readonly, strong) NSValue *homography;

/** The dimensions of the reference image that matched.
 *
 * If not `nil` the dimensions are returned as a `CGSize` instance which means you can get them
 * as follow:
 *
 * `CGSize dimensions = [result.dimensions CGSizeValue];`
 *
 * The returned `CGSize` corresponds to the width and height in pixels of the reference image
 * as it has been indexed, resized if necessary so that its greatest dimension is <= 480px.
 * For example, if you indexed a 1000 x 1500 image, this property will return a 320 x 480 CGSize.
 *
 * In practice, this is useful to get the aspect ratio of the recognized image in order to
 * un-normalize the `homography` property. This property is also internally used by the
 * [MSResult warpImage:block:] and [MSResult warpImage:scale:block:] methods.
 *
 * @return Returns the `NSValue` containing the dimensions of the reference image, or `nil` if
 * the dimensons are not available or have not been requested with the `MSResultExtraDimensions`
 * flag.
 *
 * @warning **Important:** the reference image dimensions are only available for results of type
 * `MSResultTypeImage` with `MSResultOriginClient` origin.
 */
@property (nonatomic, readonly, strong) NSValue *dimensions;

///---------------------------------------------------------------------------------------
/// @name Image Warping
///---------------------------------------------------------------------------------------

/** Warps the image associated with this result.
 *
 * This methods crops and straightens up the detected region of interest from the input image
 * and gives it the aspect ratio of the original reference image, while keeping the best possible
 * resolution.
 *
 * It can be used to pipe the results of the scanner to any third-party library requesting high quality
 * frames, such as an Optical Character Recognition (OCR) library.
 *
 * @param image the image associated with this result.
 * @param resultBlock the block that is called when the warping is done. If the input image cannot been
 * warped because this result lacks of homography and reference image dimensions, `warpedImage` is `nil`.
 *
 * @warning **Important:** this method is only available if the homography and reference image dimensions
 * have properly been requested using the `MSResultExtraHomography` and `MSResultExtraDimensions` flags.
 *
 * @warning **Note:** warping an image could be CPU consuming so this methods operates in the background.
 * The result block is called on the main thread.
 */
- (void)warpImage:(UIImage *)image
            block:(void (^)(UIImage *warpedImage))resultBlock;

/** Warps the input image associated with this result.
 *
 * Same as warpImage:block: except that this method rescale the warped image with a given scaling factor.
 *
 * @param image the image associated with this result.
 * @param scale the scaling factor to be applied, in the [0, 1] range. If not in this range, it is clamped
 * to fit into it.
 * @param resultBlock the block that is called when the warping is done. If the input image cannot been
 * warped because this result lacks of homography and reference image dimensions, `warpedImage` is `nil`.
 */
- (void)warpImage:(UIImage *)image
            scale:(float)scale
            block:(void (^)(UIImage *warpedImage))resultBlock;

///---------------------------------------------------------------------------------------
/// @name Base64 URL Decoding
///---------------------------------------------------------------------------------------

/** Perform decoding with the base64url without padding scheme.
 *
 * This is a convenient utility that you can use when you have to deal with encoded scan
 * results. This is typically the case when you need to index on Moodstocks API images with
 * identifiers containing special characters: in such a case you must encode your ID-s with
 * base64url without padding first.
 *
 * @param string the string to be decoded.
 * @return the decoded data as a byte array.
 */
+ (NSData *)dataFromBase64URLString:(NSString *)string;

@end
