map = Map("iot-hw")

section = map:section(NamedSection, "main_sct", "iot-hw", "Main section")

flag = section:option(Flag, "enable", "Enable", "Enable iot-hw sevice")

org = section:option( Value, "org", "Organization ID")
org.datatype = "string"
org.rmempty = false

type = section:option( Value, "type", "Device type")
type.datatype = "string"
type.rmempty = false

id = section:option( Value, "id", "Device ID")
id.datatype = "string"
id.rmempty = false

token = section:option( Value, "token", "Token")
token.password = true
token.datatype = "string"

return map