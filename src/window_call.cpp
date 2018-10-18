#include "window_call.hpp"

#include <glfw/glfw3.h>

#include <iostream>

namespace ppr
{
	window_callbacks& window_callbacks::get()
	{
		static window_callbacks instance;

		return instance;
	}

	void window_callbacks::init(GLFWwindow* a_window)
	{
		static bool initialized = false;

		if (!initialized)
		{
            if (a_window == nullptr)
                throw Error("Provided window was null pointer.", Error::Code::NULL_PTR);

			printf("Initializing window callbacks...\n");

			glfwSetWindowSizeCallback(a_window, on_resize);
			glfwSetWindowCloseCallback(a_window, on_close);
			glfwSetWindowFocusCallback(a_window, on_focus);
			glfwSetWindowPosCallback(a_window, on_reposition);
			glfwSetWindowRefreshCallback(a_window, on_refresh);
			glfwSetWindowIconifyCallback(a_window, on_iconify);

			initialized = true;

			printf("window callbacks initialized.\n\n");
		}
	}

	void window_callbacks::on_resize(GLFWwindow* a_window, int a_width, int a_height)
	{
		get().execute(call_type::RESIZE);
	}

	void window_callbacks::on_close(GLFWwindow* a_window)
	{
		get().execute(call_type::CLOSE);
	}

	void window_callbacks::on_focus(GLFWwindow* a_window, cbool was_focused)
	{
		if (was_focused != GLFW_TRUE && was_focused != GLFW_FALSE)
			printf(std::string(std::string("\nWarning: 'was_focused' in 'window_callbacks::on_focus(GLFWwindow* a_window, cbool was_focused)'") 
								+ "\nValue evaluated to: " + std::to_string(was_focused) + " (non-bool).\n\n").c_str());

		if (was_focused)
			get().execute(call_type::FOCUS);
		else
			get().execute(call_type::UNFOCUS);
	}

	void window_callbacks::on_reposition(GLFWwindow* a_window, int x, int y)
	{
		get().execute(call_type::REPOSITION);
	}

	void window_callbacks::on_refresh(GLFWwindow* a_window)
	{
		get().execute(call_type::REFRESH);
	}

	void window_callbacks::on_iconify(GLFWwindow* a_window, cbool was_iconified)
	{
		get().execute(call_type::ICONIFY);
	}

	void window_callbacks::execute(call_type a_type)
	{
		for (auto& i_callback : m_callbacks)
		{
			if (i_callback.second == a_type)
				i_callback.first->execute();
		}
	}
}