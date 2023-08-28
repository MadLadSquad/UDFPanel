#pragma once
#include <Framework.hpp>
#include <Generated/Config.hpp>

namespace udf_panel
{
    struct UIMGUI_PUBLIC_API Font
    {
        UImGui::FString name;
        UImGui::FString location;

        float size = 12.0f;

        ImFont* font = nullptr;
    };

    struct UIMGUI_PUBLIC_API ExecModule
    {
        UImGui::FString name;

        UImGui::FString command;
        UImGui::FString onClick;
        UImGui::FString onDoubleClick;

        int column = 0;

        Font* font = nullptr;

        UImGui::FString internalBuffer;
    };

    enum ModuleType
    {
        UDF_PANEL_EXEC_MODULE,
        UDF_PANEL_CUSTOM_MODULE
    };

    struct UIMGUI_PUBLIC_API Module
    {
        void* data = nullptr;
        ModuleType type = UDF_PANEL_EXEC_MODULE;
        float refreshAfter = -1;

        // Internal data
        float timer = 0.0f;
        bool bShouldRefresh = true;
    };

    class UIMGUI_PUBLIC_API Instance : public UImGui::Instance
    {
    public:
        Instance();
        virtual void begin() override;
        virtual void tick(float deltaTime) override;
        virtual void end() override;
        virtual ~Instance() override;

        virtual void onEventConfigureStyle(ImGuiStyle& style, ImGuiIO& io) override;

        std::vector<Font> fonts;
    private:
        // TODO: Add plugin modules
        std::vector<ExecModule> execModules;
        std::vector<std::vector<Module>> modules;

        float topMargin = 0;
        UImGui::FVector2 position = { 0.0f, 0.0f };
        UImGui::FVector2 size = { 0.0f, 0.0f };

        int columns = 1;

        void loadModules() noexcept;
        static void executeCommand(ExecModule& module, char* command, bool& bShouldRefresh) noexcept;
    };
}

