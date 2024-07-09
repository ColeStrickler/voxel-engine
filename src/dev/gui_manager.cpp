#include "gui_manager.h"
#include "renderer.h"
GUI GUI_Manager;
extern Renderer renderer;
extern Logger logger;
LogTarget GUI::m_LogTarget;
bool GUI::m_bRunLogThread;
std::thread GUI::m_LogThread;




bool Vec3Slider(glm::vec3* v, const std::string& x_label, const std::string& y_label, const std::string& z_label)
{
    bool ret = false;
    ret = ret || ImGui::SliderFloat(x_label.c_str(),    &v->x, -100.0f, 100.0f);
    ret = ret || ImGui::SliderFloat(y_label.c_str(),    &v->y, -100.0f, 100.0f);
    ret = ret || ImGui::SliderFloat(z_label.c_str(),    &v->z, -100.0f, 100.0f);
    return ret;
}


bool RgbSlider(glm::vec3* v)
{

    bool ret = false;
    ret = ret || ImGui::SliderFloat("r",    &v->x, 0.0f, 1.0f);
    ret = ret || ImGui::SliderFloat("g",    &v->y, 0.0f, 1.0f);
    ret = ret || ImGui::SliderFloat("b",    &v->z, 0.0f, 1.0f);
    return ret;
}




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
   // ImGui::Begin();
}

void GUI::End()
{
   // ImGui::End();
    // Renders the ImGUI elements
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GUI::HandleObjectSelection(RenderObject *obj)
{
    if (m_CurrentObject == nullptr)
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
    ImVec2 tabBarSize(ImGui::GetIO().DisplaySize.x, 100);
    ImGui::SetNextWindowPos(ImVec2(0, ImGui::GetIO().DisplaySize.y - tabBarSize.y), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, tabBarSize.y), ImGuiCond_Always);
    ImGui::Begin("Log");
    auto logs = m_LogTarget.LogQueue;

    for (auto& log: logs)
    {
        ImGui::Text("%s", log.c_str());
    }
    ImGui::End();
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

        //HandleObjectPositionOptions();
        if(Vec3Slider(&m_CurrentObject->m_Position, "posX", "posY", "posZ"))
            m_CurrentObject->SetPosition(m_CurrentObject->GetPosition());

        if (ImGui::Button("Duplicate"))
        {
            renderer.AddRenderObject(m_CurrentObject->Duplicate());
            logger.Log(LOGTYPE::INFO, "Added new RenderObject to Renderer.");
        }

        switch(m_CurrentObject->GetType())
        {
            case OBJECTYPE::RegularMaterial:
            {
                ImGui::Text("Material Options");
                // Select material combo box
                if(ImGui::Combo("Materials", &m_CurrentObject->m_MaterialId, [](void* data, int idx, const char** out_text) {
                    *out_text = materials[idx].name.c_str(); return true;}, (void*)NUM_MATERIALS, NUM_MATERIALS))
                        m_CurrentObject->m_Material = materials[m_CurrentObject->m_MaterialId];
                break;
            }
            case OBJECTYPE::PointLightSource:
            {
                ImGui::Text("Light Source Options");
              //  RgbSlider(&m_CurrentObject->m_Light.color);
                Vec3Slider(&m_CurrentObject->m_Light.direction, "lightDirX", "lightDirY", "lightDirZ");
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
