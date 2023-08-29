#include "MainView.hpp"
#include "Instance.hpp"

udf_panel::MainView::MainView()
{

}

void udf_panel::MainView::begin()
{
    beginAutohandle();
    inst = static_cast<udf_panel::Instance*>(UImGui::Instance::getGlobal());
}

void udf_panel::MainView::tick(float deltaTime)
{
    tickAutohandle(deltaTime);
    ImGui::SetCursorPos({ 0.0f, inst->topMargin});

    if (ImGui::BeginTable("table", inst->columns))
    {
        for (size_t i = 0; i < inst->columns; i++)
        {
            ImGui::TableNextColumn();
            for (auto& a : inst->modules[i])
            {
                // Update the module timer
                a.timer += (deltaTime);
                if (a.type == UDF_PANEL_EXEC_MODULE && static_cast<ExecModule*>(a.data)->column == i)
                {
                    auto module = static_cast<ExecModule*>(a.data);
                    ImGui::PushFont(module->font->font);

                    if (a.refreshAfter >= 0 && a.timer >= a.refreshAfter)
                    {
                        a.timer = 0.0f;
                        a.bShouldRefresh = true;
                    }
                    if (a.bShouldRefresh)
                        executeCommand(*module, module->command.data(), a.bShouldRefresh);

                    ImGui::Text("%s %f", module->internalBuffer.c_str(), deltaTime);
                    if (ImGui::IsItemHovered())
                    {
                        bool tmp = true;
                        if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                            executeCommand(*module, module->onDoubleClick.data(), tmp);
                        else if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
                            executeCommand(*module, module->onClick.data(), tmp);
                    }
                    ImGui::PopFont();
                }
                else if (a.type == UDF_PANEL_CUSTOM_MODULE)
                {

                }
            }
        }
        ImGui::EndTable();
    }
}

void udf_panel::MainView::end()
{
    endAutohandle();

}

udf_panel::MainView::~MainView()
{

}

void udf_panel::MainView::executeCommand(udf_panel::ExecModule& module, char* command, bool& bShouldRefresh) noexcept
{
    module.internalBuffer.clear();
    module.internalBuffer.resize(256);

    char* const cmds[] = { (char*)"bash", (char*)"-c", command, nullptr };
    uexec::ScriptRunner runner{};
    runner.init(cmds, false, true, false);

    size_t totalBytes = 0;
    while (!runner.finished())
    {
        size_t bytesWritten = 0;
        runner.readSTDOUT(module.internalBuffer, 256, bytesWritten);
        totalBytes += bytesWritten;
    }
    module.internalBuffer.resize(totalBytes);

    runner.destroy();
    bShouldRefresh = false;
}