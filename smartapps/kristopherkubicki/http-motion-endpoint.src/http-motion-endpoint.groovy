/**
 *  Camera Motion
 *
 *  Copyright 2015 Kristopher Kubicki
 *
 */
definition(
    name: "HTTP Motion Endpoint",
    namespace: "KristopherKubicki",
    author: "kristopher@acm.org",
    description: "Creates an HTTP motion endpoint for your camera or other device",
    category: "Convenience",
    iconUrl: "https://s3.amazonaws.com/smartapp-icons/Meta/intruder_motion-cam.png",
    iconX2Url: "https://s3.amazonaws.com/smartapp-icons/Meta/intruder_motion-cam@2x.png")


preferences {
    page(name: "selectDevices", install: false, uninstall: true, nextPage: "viewURL") {
    	section("Allow endpoint to control this thing...") {
			input "motions", "capability.motionSensor", title: "Which simulated motion sensor?"
			label title: "Assign a name", required: false
			mode title: "Set for specific mode(s)", required: false
        }
    }
    page(name: "viewURL", title: "viewURL", install: true)
}

def installed() {
	log.debug "Installed with settings: ${settings}"
}

def updated() {
	log.debug "Updated with settings: ${settings}"
	unsubscribe()

}



mappings {

    path("/active") {
        action: [
            GET: "activeMotion"
        ]
    }
    path("/inactive") {
        action: [
            GET: "inactiveMotion"
        ]
    }
}

void activeMotion() {
	log.debug "Updated with settings: ${settings}"
    motions?.active()
    runIn(15,"inactiveMotion")
}

void inactiveMotion() {
	log.debug "Updated with settings: ${settings}"
    motions?.inactive()
}


def generateURL() {
	
	createAccessToken()
	
	["https://graph.api.smartthings.com/api/smartapps/installations/${app.id}/active", "?access_token=${state.accessToken}"]
}


def viewURL() {
	dynamicPage(name: "viewURL", title: "HTTP Motion Endpoint", install:!resetOauth, nextPage: resetOauth ? "viewURL" : null) {
			section() {
                generateURL() 
				paragraph "Activate: https://graph.api.smartthings.com/api/smartapps/installations/${app.id}/active?access_token=${state.accessToken}"
                paragraph "Deactivate: https://graph.api.smartthings.com/api/smartapps/installations/${app.id}/inactive?access_token=${state.accessToken}"

			}
	}
}