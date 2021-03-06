#include "globals.hpp"
#include "util.hpp"
#include "debugger.hpp"
#include "logger.hpp"

// leave this largely in C mode Vulkan-hpp does not work very well when it comes to the debugging features

namespace ppr
{
	debugger::debugger(const vk::Instance& an_instance)
		: m_instance(an_instance)
	{}

    debugger::~debugger()
    {
        destroy();
    }

	void debugger::init()
	{
		if (!VALIDATION_LAYERS_ENABLED)
			return;

		log->debug("Initializing debugger callback...");

		VkDebugUtilsMessengerCreateInfoEXT createinfo;
		createinfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createinfo.flags = 0;
		createinfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT 
                                   | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT 
                                   | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createinfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT 
                               | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT 
                               | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createinfo.pfnUserCallback = callback;

        if (print(create_debug_utils_msgr(&createinfo, nullptr, &m_messenger)) != VK_SUCCESS)
            log->critical("Failed to setup: DEBUG UTILITIES MESSENGER");
	}

	void debugger::destroy()
	{
		if (m_messenger != VK_NULL_HANDLE)
			destroy_debug_utils_msgr(m_messenger);
	}

	VKAPI_ATTR VkBool32 VKAPI_CALL debugger::callback(VkDebugUtilsMessageSeverityFlagBitsEXT a_severity,
		                                              VkDebugUtilsMessageTypeFlagsEXT a_type,
		                                        const VkDebugUtilsMessengerCallbackDataEXT* a_callback_data,
		                                              void* a_user_data)
	{
        log->warn("Validation layer: {}", a_callback_data->pMessage);

		return VK_FALSE;
	}

	VkResult debugger::create_debug_utils_msgr(const VkDebugUtilsMessengerCreateInfoEXT* a_createinfo, 
                                               const VkAllocationCallbacks* an_allocator, 
                                                     VkDebugUtilsMessengerEXT* a_messenger) const
	{
		const auto function = (PFN_vkCreateDebugUtilsMessengerEXT)m_instance.getProcAddr(CREATE_DBG_MSGR_EXT);

		if (function != nullptr)
			return function(m_instance, a_createinfo, an_allocator, a_messenger);
		else
			return VK_ERROR_EXTENSION_NOT_PRESENT;
	}

	void debugger::destroy_debug_utils_msgr(VkDebugUtilsMessengerEXT a_messenger, 
                                      const VkAllocationCallbacks* an_allocator) const
	{
        const auto function = (PFN_vkDestroyDebugUtilsMessengerEXT)m_instance.getProcAddr(DESTROY_DBG_MSGR_EXT);

		if (function != nullptr)
			function(m_instance, a_messenger, an_allocator);
	}

	uint32_t debugger::enabled_layer_count() const
	{
		return static_cast<uint32_t>(m_validation_layers.size());
	}

	const char* const* debugger::enabled_layer_names() const
	{
		return m_validation_layers.data();
	}

	bool debugger::supports_validation_layers() const
	{
		log->trace("Checking for validation layer compatibility...");

        const std::vector<vk::LayerProperties> available_layers = vk::enumerateInstanceLayerProperties();

		for (const auto& i_layer : m_validation_layers)
		{
			bool was_found = false;

			for (const auto& i_properties : available_layers)
			{
				if (std::strcmp(i_layer, i_properties.layerName) == 0)
				{
					was_found = true;
					break;
				}
			}

			if (!was_found)
			{
				log->debug("{} is not supported", i_layer);
				return false;
			}

			log->debug("{} is supported", i_layer);
		}

		log->debug("All validation layers are supported.");

		return true;
	}
}