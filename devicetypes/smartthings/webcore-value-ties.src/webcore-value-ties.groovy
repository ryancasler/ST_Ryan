metadata {
 	definition (name: "webCoRE Value Ties", namespace: "SmartThings", author: "Robin Winbourne") {
 	capability "Actuator"
 	capability "Switch"
    attribute "Value1","string"
    attribute "Value2","string"
    attribute "Value3","string"
    attribute "Value4","string"
    attribute "Value5","string"
    attribute "Value6","string"
    command "changeValue1"
    command "changeValue2"
    command "changeValue3"
    command "changeValue4"
    command "changeValue5"
    command "changeValue6"
     }
 	tiles {
 		valueTile("Value1", "device.Value1", width: 3, height: 1, canChangeBackground: true) {
 			state "default", label:'${currentValue}'
 		}
 		valueTile("Value2", "device.Value2", width: 3, height: 1) {
 			state "default", label:'${currentValue}'
 		}
 		valueTile("Value3", "device.Value3", width: 3, height: 1) {
 			state "default", label:'${currentValue}'
 		}
 		valueTile("Value4", "device.Value4", width: 3, height: 1) {
 			state "default", label:'${currentValue}'
 		}
 		valueTile("Value5", "device.Value5", width: 3, height: 1) {
 			state "default", label:'${currentValue}'
 		}
 		valueTile("Value6", "device.Value6", width: 3, height: 1) {
 			state "default", label:'${currentValue}'
 		}
 		main(["Value1"])
 		details(["Value1","Value2","Value3","Value4","Value5","Value6"])
 	}
 }
 def changeValue1 (param1) {
    sendEvent("name":"Value1", "value":param1)
}
 def changeValue2 (param2) {
    sendEvent("name":"Value2", "value":param2)
}
 def changeValue3 (param3) {
    sendEvent("name":"Value3", "value":param3)
}
 def changeValue4 (param4) {
    sendEvent("name":"Value4", "value":param4)
}
 def changeValue5 (param5) {
    sendEvent("name":"Value5", "value":param5)
}
 def changeValue6 (param5) {
    sendEvent("name":"Value6", "value":param6)
}