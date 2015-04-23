import AVFoundation
import CoreGraphics
import UIKit

class MoodstocksScannerViewController: UIViewController  {
    var videoPreview: UIView = UIView()
    var scannerSession: MSAutoScannerSession

    required init(coder aDecoder: (NSCoder!)) {
        fatalError("NSCoding not supported")
    }

    init(scanner aScanner: (MSScanner!), delegate delegate: (MSAutoScannerSessionDelegate!)) {
        scannerSession = MSAutoScannerSession(scanner:aScanner)
        scannerSession.delegate = delegate
        scannerSession.resultTypes = MSResultType.Image.rawValue /// Todo: allow custom results (QR code, etc)

        super.init(nibName: nil, bundle: nil);
    }

    override func loadView() {
        super.loadView()

        self.videoPreview.frame = CGRectMake(0, 0, CGRectGetWidth(self.view.bounds), CGRectGetHeight(self.view.bounds))

        self.videoPreview.backgroundColor = UIColor.blackColor()
        self.videoPreview.autoresizingMask = UIViewAutoresizing.FlexibleWidth | UIViewAutoresizing.FlexibleHeight
        self.videoPreview.autoresizesSubviews = true

        self.view.addSubview(self.videoPreview)

        let videoPreviewLayer: CALayer  = self.videoPreview.layer
        videoPreviewLayer.masksToBounds = true

        let captureLayer: AVCaptureVideoPreviewLayer = self.scannerSession.captureLayer
        captureLayer.frame = self.view.bounds
        videoPreviewLayer.insertSublayer(captureLayer, atIndex:0)

        // This is a bit of a hack, but we want to start the camera
        // without scanning just yet to avoid scanning while
        // the view is animating
        self.scannerSession.startRunning()
        self.scannerSession.pauseProcessing()
    }

    override func viewDidAppear(animated: Bool) {
        super.viewDidAppear(animated)
        println("moodstocks: starting scanner session")
        self.scannerSession.resumeProcessing()
    }

    func updateInterfaceOrientation(interfaceOrientation: UIInterfaceOrientation) {
        let captureLayer: AVCaptureVideoPreviewLayer = self.scannerSession.captureLayer
        self.scannerSession.interfaceOrientation = interfaceOrientation
        captureLayer.frame = self.view.bounds

        switch (interfaceOrientation) {
             case UIInterfaceOrientation.Portrait:
                 captureLayer.connection.videoOrientation = AVCaptureVideoOrientation.Portrait

             case UIInterfaceOrientation.PortraitUpsideDown:
                 captureLayer.connection.videoOrientation = AVCaptureVideoOrientation.PortraitUpsideDown

             case UIInterfaceOrientation.LandscapeLeft:
                 captureLayer.connection.videoOrientation = AVCaptureVideoOrientation.LandscapeLeft

             case UIInterfaceOrientation.LandscapeRight:
                 captureLayer.connection.videoOrientation = AVCaptureVideoOrientation.LandscapeRight

            default:
                break
        }
    }

    override func viewWillLayoutSubviews() {
        self.updateInterfaceOrientation(self.interfaceOrientation)
    }

    override func willAnimateRotationToInterfaceOrientation(orientation: UIInterfaceOrientation, duration: NSTimeInterval) {
        super.willAnimateRotationToInterfaceOrientation(orientation, duration:duration)
        self.updateInterfaceOrientation(orientation)
    }

    deinit {
        println("moddstocks: stopping scanner session")
        self.scannerSession.stopRunning()
    }
}
