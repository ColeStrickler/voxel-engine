#include "gui_manager.h"

GUI GUI_Manager;

LogTarget GUI::m_LogTarget;
bool GUI::m_bRunLogThread;
std::thread GUI::m_LogThread;

GUI::GUI() : m_CurrentObject(nullptr)
{
    m_LogThread = std::thread(LoggingThread);
}

GUI::~GUI()
{
    /*
        Setting logging thread to false and notify condition variable
        so the logging thread may exit. Wait for it to complete
    */
    m_bRunLogThread = false;
    m_LogTarget.cv.notify_all();
    m_LogThread.join(); 


}

void GUI::RenderGUI()
{
    DisplayObjectOptions();
    DisplayLogs();
}

void GUI::Begin()
{
     ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void GUI::End()
{
    ImGui::End();
    // Renders the ImGUI elements
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GUI::HandleObjectSelection(RenderObject *obj)
{
    m_CurrentObject = obj;
}

void GUI::RegisterLogTarget(Logger *logger)
{
    logger->RegisterTarget(&m_LogTarget);
}

void GUI::LoggingThread()
{
    m_bRunLogThread = true;
    while (m_bRunLogThread)
    {
        std::unique_lock lock(m_LogTarget.lock);
        m_LogTarget.cv.wait(lock, []{return m_LogTarget.LogQueue.size();});
        if (m_LogTarget.LogQueue.size() > 25) // we only display the last 25 elements
            m_LogTarget.ResizeQueue(25);


       // for (auto& log : m_LogTarget.LogQueue)
        //{
       //     std::cout << log << std::endl;
        //}

        lock.unlock();
       // m_LogTarget.LogQueue.clear();
    }

}

void GUI::DisplayLogs()
{
    auto logs = m_LogTarget.LogQueue;

    for (auto& log: logs)
    {
        ImGui::Text("%s", log.c_str());
    }
}

void GUI::DisplayObjectOptions()
{
    if (!m_CurrentObject)
        return;


    /*
        Change this to the objects name
    */

    bool open = true;
    ImGui::OpenPopup("My Popup");


    if (ImGui::BeginPopupModal("My Popup", &open, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Object Editor!");
        ImGui::Separator();

        if (ImGui::Button("Wireframe"))
            m_CurrentObject->ToggleWireFrame();

        if (ImGui::Button("Close")) {
            ImGui::CloseCurrentPopup();
            m_CurrentObject = nullptr;
        }

        ImGui::EndPopup();
    }

}
