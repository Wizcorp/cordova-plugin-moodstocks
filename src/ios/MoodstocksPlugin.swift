import Foundation

@objc(MoodstocksPlugin) class MoodstocksPlugin : CDVPlugin, MSAutoScannerSessionDelegate {
	var window: UIWindow!
	var syncCallbacks = [String: String]()
	var scanner: MSScanner = MSScanner()
	var scanCallback: String!

	func configure(command: CDVInvokedUrlCommand) {
		var err: NSError?

		let path: String = MSScanner.cachesPathFor("scanner.db")

		let config = command.arguments[0] as! [String: String]
		let key: String = config["key"] ?? ""
		let secret: String = config["secret"] ?? ""

		println("moodstocks: Configuring...")
		self.scanner.openWithPath(path, key:key, secret:secret, error:&err)

		if err != nil {
			// Todo - forward error information
			let pluginResult = CDVPluginResult(status: CDVCommandStatus_ERROR)
			commandDelegate.sendPluginResult(pluginResult, callbackId:command.callbackId)
		} else {
			let pluginResult = CDVPluginResult(status: CDVCommandStatus_OK)
			commandDelegate.sendPluginResult(pluginResult, callbackId:command.callbackId)
		}
	}

	func on(command: CDVInvokedUrlCommand) {
		let action: String = command.arguments[0] as! String
		println("moodstocks: setting sync callback", action)
		self.syncCallbacks[action] = command.callbackId
	}

	func sync(command: CDVInvokedUrlCommand) {
		println("moodstocks: syncing")

		let completed: (MSSync?, NSError?) -> Void = {
		operation, err in
			if err != nil {
				println("moodstocks: sync error")

				if self.syncCallbacks["error"] != nil {
					println("moodstocks: calling sync error callback")

					let pluginResult = CDVPluginResult(status: CDVCommandStatus_ERROR)
					self.commandDelegate.sendPluginResult(pluginResult, callbackId: self.syncCallbacks["error"])
				}
			} else {
				println("moodstocks: sync completed")

				if self.syncCallbacks["completed"] != nil {
					println("moodstocks: calling sync completed callback")

					let pluginResult = CDVPluginResult(status: CDVCommandStatus_OK)
					self.commandDelegate.sendPluginResult(pluginResult, callbackId: self.syncCallbacks["completed"])
				}
			}
		}

		let progress: (NSInteger) -> Void = {
		percentDone in
			println("moodstocks: sync progress")

			if self.syncCallbacks["progress"] != nil {
				println("moodstocks: calling sync progress callback")

				let resultData = ["percent": percentDone]
				let pluginResult = CDVPluginResult(status: CDVCommandStatus_OK, messageAsDictionary: resultData)
				self.commandDelegate.sendPluginResult(pluginResult, callbackId: self.syncCallbacks["progress"])
			}
		}

		self.scanner.syncInBackgroundWithBlock(completed, progressBlock: progress)

		println("moodstocks: sync started")

		if self.syncCallbacks["start"] != nil {
			println("moodstocks: calling sync started callback")

			let pluginResult = CDVPluginResult(status: CDVCommandStatus_OK)
			self.commandDelegate.sendPluginResult(pluginResult, callbackId: self.syncCallbacks["start"])
		}
	}

	func scan(command: CDVInvokedUrlCommand) {
		println("moodstocks: scanning")

		var svc: MoodstocksScannerViewController = MoodstocksScannerViewController(scanner: self.scanner, delegate: self)
		self.scanCallback = command.callbackId
		self.viewController.presentViewController(svc, animated:true, completion: nil)
	}

	func session (scannerSession: AnyObject!, didFindResult result: MSResult!) {
		let type: NSString = (result.type == MSResultType.Image ? "Image" : "Barcode")
		println("moodstocks: Found", type, result.string)

		self.viewController.dismissViewControllerAnimated(true, completion:nil)

		let resultData = ["data": result.string]
		let pluginResult = CDVPluginResult(status: CDVCommandStatus_OK, messageAsDictionary: resultData)
		self.commandDelegate.sendPluginResult(pluginResult, callbackId: self.scanCallback)
	}
}
