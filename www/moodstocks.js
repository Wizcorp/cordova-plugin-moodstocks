/*global cordova, module*/

var PLUGIN_NAME = "MoodstocksPlugin";

module.exports = {
	configure: function (config, successCallback, errorCallback) {
		cordova.exec(successCallback, errorCallback, PLUGIN_NAME, "configure", [config]);
	},
	on: function (action, successCallback) {
		cordova.exec(successCallback, function () {
			console.error('could not set on event', arguments);
		}, PLUGIN_NAME, "on", [action]);
	},
	sync: function (successCallback, errorCallback) {
		cordova.exec(console.log, console.error, PLUGIN_NAME, "sync", []);
	},
	scan: function (successCallback, errorCallback) {
		cordova.exec(function (result) {
			try {
				result.data = window.atob(result.data);
			} catch(e) {
				console.debug('Moodstocks: Received scan result is not base64');
			}

			successCallback(result);
		}, errorCallback, PLUGIN_NAME, "scan", []);
	}
};
