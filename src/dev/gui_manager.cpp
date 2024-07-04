#include "gui_manager.h"
#include "renderer.h"
GUI GUI_Manager;
extern Renderer renderer;
extern Logger logger;
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

        HandleObjectPositionOptions();

        if (ImGui::Button("Duplicate"))
        {
            renderer.AddRenderObject(m_CurrentObject->Duplicate());
            logger.Log(LOGTYPE::INFO, "Added new RenderObject to Renderer.");
        }

        switch(m_CurrentObject->GetType())
        {
            case OBJECTYPE::Regular:
            {
                ImGui::Text("Material Options");
                // Select material combo box
            if(ImGui::Combo("Materials", &m_CurrentObject->m_MaterialId, [](void* data, int idx, const char** out_text) {
                *out_text = materials[idx].name.c_str(); return true;}, (void*)NUM_MATERIALS, NUM_MATERIALS))
                    m_CurrentObject->m_Material = materials[m_CurrentObject->m_MaterialId];
                break;
            }
            case OBJECTYPE::LightSource:
            {
                ImGui::Text("Light Source Options");
                ImGui::SliderFloat("r",    &m_CurrentObject->m_Light.color.x, 0.0f, 1.0f);
                ImGui::SliderFloat("g",    &m_CurrentObject->m_Light.color.y, 0.0f, 1.0f);
                ImGui::SliderFloat("b",    &m_CurrentObject->m_Light.color.z, 0.0f, 1.0f);
                break;
            }
        }




        if (ImGui::Button("Close")) {
            ImGui::CloseCurrentPopup();
            m_CurrentObject = nullptr;
        }

        ImGui::EndPopup();
    }

}

void GUI::HandleObjectPositionOptions()
{
    if(ImGui::SliderFloat("x", &m_CurrentObject->m_Position.x, -10.0f, 10.0f) || ImGui::SliderFloat("y", &m_CurrentObject->m_Position.y, -10.0f, 10.0f)\
        ||ImGui::SliderFloat("z", &m_CurrentObject->m_Position.z, -10.0f, 10.0f))
            m_CurrentObject->SetPosition(m_CurrentObject->GetPosition());
}
