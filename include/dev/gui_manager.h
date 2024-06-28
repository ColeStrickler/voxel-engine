#ifndef GUIMANAGER_H
#define GUIMANAGER_H
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <thread>
#include "logger.h"
class GUI
{
public:
    GUI();
    ~GUI();
    void RenderGUI();
    void Begin();
    void End();

    void RegisterLogTarget(Logger* logger);
    
private:
    static std::thread m_LogThread;
    static LogTarget m_LogTarget;
    static void LoggingThread();
    static bool m_bRunLogThread;
    void DisplayLogs();
};




#endif