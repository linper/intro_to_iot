module("luci.controller.iot-hw_controller", package.seeall)

function index()
	entry({"admin", "services", "iot-hw_model"}, cbi("iot-hw_model"), _("iot-hw"),106)
end
