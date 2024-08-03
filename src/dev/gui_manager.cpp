#include "gui_manager.h"
#include "renderer.h"
GUI GUI_Manager;
extern Renderer renderer;
extern Logger logger;
extern Profiler profiler;
LogTarget GUI::m_LogTarget;
bool GUI::m_bRunLogThread;
std::thread GUI::m_LogThread;

extern int MAX_CHUNKS;
extern float CHUNK_DISTANCE;
extern float DELETE_DISTANCE;

bool Vec3Slider(glm::vec3* v, const std::string& x_label, const std::string& y_label, const std::string& z_label, float limit = 100.0f, float floor = -100.0f)
{
    bool ret = false;
    ret = ret || ImGui::SliderFloat(x_label.c_str(),    &v->x, floor, limit);
    ret = ret || ImGui::SliderFloat(y_label.c_str(),    &v->y, floor, limit);
    ret = ret || ImGui::SliderFloat(z_label.c_str(),    &v->z, floor, limit);
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
    EMIT_PROFILE_TOKEN
    DisplayProfilerStatistics();
    DisplayObjectOptions();
    DisplayLogs();
    DisplayChunkManagementOptions();
}

void GUI::Begin()
{
    EMIT_PROFILE_TOKEN
    ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
   // ImGui::Begin();
}

void GUI::End()
{
    EMIT_PROFILE_TOKEN
   // ImGui::End();
    // Renders the ImGUI elements
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GUI::HandleObjectSelection(RenderObject *obj)
{
    if (m_CurrentObject == nullptr)
    {
        m_CurrentObject = obj;
        m_ObjectOptions.clear();
    }
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

void GUI::DisplayProfilerStatistics()
{
    EMIT_PROFILE_TOKEN

    if (!m_bShowProfilerStatistics)
        return;

    std::vector<std::pair<std::string, double>> entries(profiler.m_TimeAccounting.begin(), profiler.m_TimeAccounting.end());
    std::sort(entries.begin(), entries.end(),
        [](const std::pair<std::string, double>& a, const std::pair<std::string, double>& b) {
            return a.second < b.second; // Ascending order by value
        });

    bool open = true;
    ImGui::OpenPopup("Function Profiling Statistics");


    if (ImGui::BeginPopupModal("Function Profiling Statistics", &open, ImGuiWindowFlags_None)) {

         ImGui::SetWindowSize(ImVec2(600, 400));
        ImGui::Columns(2, "myColumns", true); // true: Show column borders

        ImGui::SetColumnWidth(0, 300.0f); // Set width of the first column
        ImGui::SetColumnWidth(1, 300.0f); // Set width of the second column


        ImGui::Text("Function");
        ImGui::NextColumn();
        ImGui::Text("Time (s)");
        ImGui::NextColumn();
        ImGui::Separator();


        for (int i = 0; i < std::min(size_t(50), entries.size()); i++)
        {
            ImGui::Text(entries[i].first.c_str());
            ImGui::NextColumn();
            ImGui::Text("%.4f", entries[i].second);
            ImGui::NextColumn();
        }
        ImGui::Columns(1); // Resets the columns to default single column layout

        if (ImGui::Button("Close")) {
            ImGui::CloseCurrentPopup();
            m_bShowProfilerStatistics = false;
        }

        ImGui::EndPopup();
    }
  
}

void GUI::DisplayChunkManagementOptions()
{
    if (!m_bShowChunkManagementOptions)
        return;

    bool open = true;
    ImGui::OpenPopup("Function Profiling Statistics");


    if (ImGui::BeginPopupModal("Function Profiling Statistics", &open, ImGuiWindowFlags_None)) {

        ImGui::SetWindowSize(ImVec2(600, 400));
        ImGui::SliderInt("Max Chunks", &MAX_CHUNKS, 0, 1000);
        ImGui::SliderFloat("Chunk Distance", &CHUNK_DISTANCE, 0.0f, 16.0f);
        ImGui::SliderFloat("Delete Distance", &DELETE_DISTANCE, 0.0f, 16.0f);

        if (ImGui::Button("Close")) {
            ImGui::CloseCurrentPopup();
            m_bShowChunkManagementOptions = false;
        }

        ImGui::EndPopup();
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

        //HandleObjectPositionOptions();
        if(Vec3Slider(&m_CurrentObject->m_Position, "posX", "posY", "posZ"))
            m_CurrentObject->SetPosition(m_CurrentObject->GetPosition());

        Vec3Slider(&m_ObjectOptions.m_RotationAxis, "rotX", "rotY", "rotZ", 1.0f, 0.0f);
        ImGui::SliderFloat("Rotation Magnitude", &m_ObjectOptions.m_RotationMagnitude, 0.0f, 90.0f);
        
        if (ImGui::Button("Rotate"))
            m_CurrentObject->Rotate(m_ObjectOptions.m_RotationAxis, m_ObjectOptions.m_RotationMagnitude);
        
        ImGui::SliderFloat("Scale Magnitude", &m_ObjectOptions.m_ScaleMagnitude, 0.1f, 10.0f);
        if (ImGui::Button("Scale"))
            m_CurrentObject->Scale(m_ObjectOptions.m_ScaleMagnitude);

        if (ImGui::Button("Stencil Outline"))
        {
            m_CurrentObject->ToggleStencilOutline();
        }

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
