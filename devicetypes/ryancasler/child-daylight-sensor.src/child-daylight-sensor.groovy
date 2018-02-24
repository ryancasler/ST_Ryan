/**
 *  Child Furnace Sensor
 *
 *  Copyright 2018 Ryan Casler
 
 *   
 */
metadata {
	definition (name: "Child Daylight Sensor", namespace: "ryancasler", author: "Ryan Casler") {
		capability "Contact Sensor"
		capability "Sensor"

		attribute "lastUpdated", "String"

		command "generateEvent", ["string", "string"]
	}

	tiles(scale: 2) {
		multiAttributeTile(name:"contact", type: "generic"){
			tileAttribute ("device.contact", key: "PRIMARY_CONTROL") {
				attributeState "closed", label:'daylight', icon:"st.Weather.weather14", backgroundColor:"#e86d13"
				attributeState "open", label:'dark', icon:" st.Weather.weather4", backgroundColor:"#cccccc"
            }
 			tileAttribute("device.lastUpdated", key: "SECONDARY_CONTROL") {
    				attributeState("default", label:'    Last updated ${currentValue}',icon: "st.Health & Wellness.health9")
            }
        }
	}

}

def generateEvent(String name, String value) {
	//log.debug("Passed values to routine generateEvent in device named $device: Name - $name  -  Value - $value")
	// Update device
	sendEvent(name: name, value: value)
   	 // Update lastUpdated date and time
    def nowDay = new Date().format("MMM dd", location.timeZone)
    def nowTime = new Date().format("h:mm a", location.timeZone)
    sendEvent(name: "lastUpdated", value: nowDay + " at " + nowTime, displayed: false)
}