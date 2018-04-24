/**
 *  Generic HTTP Device v1.0.20160402
 *
 *  Source code can be found here: https://github.com/JZ-SmartThings/SmartThings/blob/master/Devices/Generic%20HTTP%20Device/GenericHTTPDevice.groovy
 *
 *  Copyright 2016 JZ
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except
 *  in compliance with the License. You may obtain a copy of the License at:
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed
 *  on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License
 *  for the specific language governing permissions and limitations under the License.
 *
 */

metadata {
	definition (name: "LED Virtual Remote", author: "Ryan Casler", namespace:"ryancasler") {
		capability "Switch"
        capability "Actuator"
		capability "Switch Level"
        attribute "triggerswitch", "string"
		command "DeviceTrigger"
	capability "Button"
	capability "Configuration"
	capability "Sensor"
	
       command "push1"
        command "push2"
        command "push3"
        command "push4"
        command "push5"
        command "push6"
        command "push7"
        command "push8"
        command "push9"
        command "push10"
	}


simulator {
	}

	tiles {
    	
    		standardTile("button", "device.button") {
			state "default", label: "", icon: "st.unknown.zwave.remote-controller", backgroundColor: "#ffffff"
		}
    multiAttributeTile(name:"DeviceTrigger", type:"generic", width:6, height:4) {
			tileAttribute("device.triggerswitch", key: "PRIMARY_CONTROL") {
				attributeState "triggeroff", label:'CLOSED' , action: "on", icon: "st.Home.home9", backgroundColor:"#ffffff", nextState: "trying"
				attributeState "triggeron", label: 'OPEN', action: "off", icon: "st.Home.home9", backgroundColor: "#79b821", nextState: "trying"
				attributeState "trying", label: 'TRYING', action: "", icon: "st.Home.home9", backgroundColor: "#FFAA33"
			}
			tileAttribute("device.level", key: "SLIDER_CONTROL") {
				attributeState "level", action:"switch level.setLevel", defaultState: true
			}
		}

 		standardTile("push1", "device.button", width: 1, height: 1, decoration: "flat") {
			state "default", label: "Off", backgroundColor: "#ffffff", action: "push1"
		}
 		standardTile("push2", "device.button", width: 1, height: 1, decoration: "flat") {
			state "default", label: "Low Warm", backgroundColor: "#ffffff", action: "push2"
		}
 		standardTile("push3", "device.button", width: 1, height: 1, decoration: "flat") {
			state "default", label: "High Warm", backgroundColor: "#ffffff", action: "push3"
		}
 		standardTile("push4", "device.button", width: 1, height: 1, decoration: "flat") {
			state "default", label: "Red Larson", backgroundColor: "#ffffff", action: "push4"
		}
 		standardTile("push5", "device.button", width: 1, height: 1, decoration: "flat") {
			state "default", label: "Rainbow", backgroundColor: "#ffffff", action: "push5"
		}
 		standardTile("push6", "device.button", width: 1, height: 1, decoration: "flat") {
			state "default", label: "Blue Dual", backgroundColor: "#ffffff", action: "push6"
		}
 		standardTile("push7", "device.button", width: 1, height: 1, decoration: "flat") {
			state "default", label: "Rainbow Chase BO", backgroundColor: "#ffffff", action: "push7"
		}
 		standardTile("push8", "device.button", width: 1, height: 1, decoration: "flat") {
			state "default", label: "Circus", backgroundColor: "#ffffff", action: "push8"
		}
 		standardTile("push9", "device.button", width: 1, height: 1, decoration: "flat") {
			state "default", label: "Red Breath", backgroundColor: "#ffffff", action: "push9"
		}
 		standardTile("push10", "device.button", width: 1, height: 1, decoration: "flat") {
			state "default", label: "Rainbow Theater Chase", backgroundColor: "#ffffff", action: "push10"
        }
        main "button"
		details(["push1","push2","button","push3","push4","push5","push6","push7","push9","push10"])
	}
}
def push1() {
	push(1)
}

def push2() {
	push(2)
}

def push3() {
	push(3)
}

def push4() {
	push(4)
}

def push5() {
	push(5)
}

def push6() {
	push(6)
}
def push7() {
	push(7)
}
def push8() {
	push(8)
}
def push9() {
	push(9)
}
def push10() {
	push(10)
}


private push(button) {
	log.debug "$device.displayName button $button was pushed"
	sendEvent(name: "button", value: "pushed", data: [buttonNumber: button], descriptionText: "$device.displayName button $button was pushed", isStateChange: true)
}

def installed() {
	initialize()
}

def updated() {
	initialize()
}

def initialize() {
	sendEvent(name: "numberOfButtons", value: 10)
}
def on() {
    log.debug "Triggered OPEN!!!"
    def myLevel = device.latestValue("level")
	sendEvent(name: "triggerswitch", value: "triggeron", isStateChange: true)
    state.blinds = "on";
	runCmd("${myLevel}")
}
def off() {
	log.debug "Triggered CLOSE!!!"
	sendEvent(name: "triggerswitch", value: "triggeroff", isStateChange: true)
    state.blinds = "off";
	runCmd("100")
}

def setLevel(percent) {
	log.debug "setLevel: ${percent}, this"
	sendEvent(name: "triggerswitch", value: "triggeron", isStateChange: true)
    state.blinds = "on";
    sendEvent(name: "level", value: percent)
    runCmd("${percent}")

}    


def parse(String description) {
	//log.debug "Parsing '${description}'"
	def whichTile = ''	
	log.debug "state.blinds " + state.blinds
	
    if (state.blinds == "on") {
    	//sendEvent(name: "triggerswitch", value: "triggergon", isStateChange: true)
        whichTile = 'mainon'
    }
    if (state.blinds == "off") {
    	//sendEvent(name: "triggerswitch", value: "triggergoff", isStateChange: true)
        whichTile = 'mainoff'
    }
	
    //RETURN BUTTONS TO CORRECT STATE
	log.debug 'whichTile: ' + whichTile
    switch (whichTile) {
        case 'mainon':
			def result = createEvent(name: "switch", value: "on", isStateChange: true)
			return result
        case 'mainoff':
			def result = createEvent(name: "switch", value: "off", isStateChange: true)
			return result
        default:
			def result = createEvent(name: "testswitch", value: "default", isStateChange: true)
			//log.debug "testswitch returned ${result?.descriptionText}"
			return result
    }
}