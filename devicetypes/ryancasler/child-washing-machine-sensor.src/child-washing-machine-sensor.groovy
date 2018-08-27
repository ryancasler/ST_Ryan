/**
 *  Child Washing Machine Sensor
 *
 *  Copyright 2018 Ryan Casler
 
 *   
 */
metadata {
	definition (name: "Child Washing Machine Sensor", namespace: "ryancasler", author: "Ryan Casler") {
		capability "Contact Sensor"
		capability "Sensor"

		attribute "lastUpdated", "String"
	}

	tiles(scale: 2) {
		multiAttributeTile(name:"contact", type: "generic"){
			tileAttribute ("device.contact", key: "PRIMARY_CONTROL") {
				attributeState "closed", label:'inactive', icon:"st.Appliances.appliances1", backgroundColor:"#cccccc"
				attributeState "open", label:'done', icon:"st.Appliances.appliances1", backgroundColor:"#e86d13"
            }
 			tileAttribute("device.lastUpdated", key: "SECONDARY_CONTROL") {
    				attributeState("default", label:'    Last updated ${currentValue}',icon: "st.Health & Wellness.health9")
            }
        }
	}

}

def parse(String description) {
    log.debug "parse(${description}) called"
	def parts = description.split(" ")
    def name  = parts.length>0?parts[0].trim():null
    def value = parts.length>1?parts[1].trim():null
    if (name && value) {
	// Update device
	sendEvent(name: name, value: value)
   	 // Update lastUpdated date and time
    def nowDay = new Date().format("MMM dd", location.timeZone)
    def nowTime = new Date().format("h:mm a", location.timeZone)
    sendEvent(name: "lastUpdated", value: nowDay + " at " + nowTime, displayed: false)
}
    else {
    	log.debug "Missing either name or value.  Cannot parse!"
    }
}

def installed() {
}