#pragma once
#include <Framework.hpp>
#include <Generated/Config.hpp>

namespace udf_panel 
{
    class Instance;
    struct ExecModule;

    class UIMGUI_PUBLIC_API MainView final : public UImGui::InlineComponent
    {
    public:
        MainView() noexcept = default;
        virtual void begin() noexcept override;
        virtual void tick(float deltaTime) noexcept override;
        virtual void end() noexcept override;
        virtual ~MainView() noexcept override = default;
    private:
        Instance* inst = nullptr;

        static void executeCommand(ExecModule& module, char* command, bool& bShouldRefresh) noexcept;
    };
}
