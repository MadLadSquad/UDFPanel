#include "MainView.hpp"
#include "Instance.hpp"

void udf_panel::MainView::begin() noexcept
{
    beginAutohandle();
    inst = static_cast<udf_panel::Instance*>(UImGui::Instance::getGlobal());
}

void udf_panel::MainView::tick(const float deltaTime) noexcept
{
    tickAutohandle(deltaTime);
    ImGui::SetCursorPos({ 0.0f, inst->topMargin});

    if (ImGui::BeginTable("table", inst->columns))
    {
        ImGui::TableNextColumn();
        for (size_t i = 0; i < inst->columns; i++)
        {
            ImGui::BeginGroup();
            for (auto& a : inst->modules[i])
            {
                // Update the module timer
                a.timer += (deltaTime);
                if (a.type == UDF_PANEL_EXEC_MODULE && static_cast<ExecModule*>(a.data)->column == i)
                {
                    const auto module = static_cast<ExecModule*>(a.data);
                    ImGui::PushFont(module->font->font);
                    //ImGui::PushID(module->name.c_str());

                    if (a.refreshAfter >= 0 && a.timer >= a.refreshAfter && a.bShouldRefresh == false)
                    {
                        a.timer = 0.0f;
                        a.bShouldRefresh = true;
                    }
                    if (a.bShouldRefresh)
                        executeCommand(*module, module->command.data(), a.bShouldRefresh);

                    ImGui::Text("%s", module->internalBuffer.c_str());
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
            ImGui::EndGroup();
            ImGui::TableNextColumn();
        }
        ImGui::EndTable();
    }
}

void udf_panel::MainView::end() noexcept
{
    endAutohandle();

}

void udf_panel::MainView::executeCommand(udf_panel::ExecModule& module, char* command, bool& bShouldRefresh) noexcept
{
    if (!module.internalBuffer.empty())
        module.internalBuffer.clear();
    module.internalBuffer.resize(256);

    char* const cmds[] = { (char*)"bash", (char*)"-c", command, nullptr };
    uexec::ScriptRunner runner{};
    runner.init(cmds, false, true, false);
    runner.update();

    size_t totalBytes = 0;
    while (!runner.finished())
    {
        size_t bytesWritten = 0;
        runner.readSTDOUT(module.internalBuffer, 256, bytesWritten);
        totalBytes += bytesWritten;
        runner.update();
    }
    module.internalBuffer.resize(totalBytes);
    runner.destroy();
    bShouldRefresh = false;
}