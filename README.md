Moodstocks Cordova Plugin
=========================

Note
----

The iOS part of this plugin is written in Swift; please
make sure to set `IPHONEOS_DEPLOYMENT_TARGET` to at least 7.0 under
your `platforms/ios/[YOUR-PROJECT].xcodeproj/project.pbxproj`.

You will also need to add a bridging header file, so that this plugin may
see Cordova's Obj-C code. Simply follow the instructions here:

[https://bohemianpolymorph.wordpress.com/2014/07/11/manually-adding-a-swift-bridging-header/](https://bohemianpolymorph.wordpress.com/2014/07/11/manually-adding-a-swift-bridging-header/)

And in the header file, make sure the following lines are present:

```objective-c
#import <Cordova/CDV.h>
#import <Moodstocks/Moodstocks.h>
```

If you get a fatal error like the following at runtime:

```
dyld: Library not loaded: @rpath/libswiftCore.dylib
  Referenced from: /private/var/mobile/Containers/Bundle/Application/.../my.app/my
    Reason: image not found
```

Please have a look at [http://stackoverflow.com/questions/24002836/dyld-library-not-loaded-rpath-libswift-stdlib-core-dylib](http://stackoverflow.com/questions/24002836/dyld-library-not-loaded-rpath-libswift-stdlib-core-dylib) to fix the issue. Namely:

1. Make sure `Runpath Search Path` includes `@executable_path/Frameworks`
2. Make sure `Embedded Content Contains Swift Code` is set to `Yes`

Built on
--------

* Moodstock iOS SDK 4.1.5
* Moodstock Android SDK 4.1.5 (Eclipse)

Installation
------------

```
cordova plugin add https://github.com/Wizcorp/cordova-plugin-moodstocks.git
```

API Documentation
------------------

### moodstocks.configure({ key: string, secret: string })

* key: Moodstocks sey
* secret: Moodstocks secret

### moodstocks.on("action", callback, errCallback)

Sync events

#### start, cb()

#### progress, cb({ percent:int })

#### completed. cb()

#### error, cb({ message: string })

### moodstocks.scan(cb({ data: string }), errCb({ message: string }))
