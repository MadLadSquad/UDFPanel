#pragma once
#include <Framework.hpp>
#include <Generated/Config.hpp>

namespace udf_panel 
{
    class Instance;
    struct ExecModule;

    class UIMGUI_PUBLIC_API MainView  : public UImGui::InlineComponent 
    {
    public:
        MainView ();
        virtual void begin() override;
        virtual void tick(float deltaTime) override;
        virtual void end() override;
        virtual ~MainView () override;
    private:
        Instance* inst;

        static void executeCommand(ExecModule& module, char* command, bool& bShouldRefresh) noexcept;
    };
}
