/**
 *  Copyright 2015 SmartThings
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
	definition (name: "LED Mode selector - Desk", namespace: "ryancasler	", author: "Ryan Casler") {
		capability "Actuator"
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

	tiles {
		standardTile("button", "device.button") {
			state "default", label: "", icon: "st.unknown.zwave.remote-controller", backgroundColor: "#ffffff"
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
			state "default", label: "Ultra Low Warm", backgroundColor: "#ffffff", action: "push4"
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
			state "default", label: "Blue Breath", backgroundColor: "#ffffff", action: "push8"
		}
 		standardTile("push9", "device.button", width: 1, height: 1, decoration: "flat") {
			state "default", label: "Brightness Up", backgroundColor: "#ffffff", action: "push9"
		}
 		standardTile("push10", "device.button", width: 1, height: 1, decoration: "flat") {
			state "default", label: "Brightness Down", backgroundColor: "#ffffff", action: "push10"
        }
        main "button"
		details(["push1","push2","button","push3","push4","push5","push6","push7","push8","push9","push10"])
	}
}

def parse(String description) {

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