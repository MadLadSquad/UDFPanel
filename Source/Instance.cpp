#include "Instance.hpp"

udf_panel::Instance::Instance()
{
    initInfo =
    {
        .inlineComponents = { &mainView },
        .globalData = this,
        .bGlobalAllocatedOnHeap = false,
        UIMGUI_INIT_INFO_DEFAULT_DIRS,
    };
}

void udf_panel::Instance::begin()
{
    beginAutohandle();

    // Load module config
    loadModules();

    // Sets up the window to be recognised as a panel
    UImGui::Window::Platform::setWindowAlwaysOnTop();
    UImGui::Window::Platform::setWindowType(X11_WINDOW_TYPE_DOCK);
    UImGui::Window::Platform::setWindowShowingOnPager(false);
    UImGui::Window::Platform::setWindowShowingOnTaskbar(false);

    for (auto& font : fonts)
        font.font = ImGui::GetIO().Fonts->AddFontFromFileTTF(font.name.c_str(), font.size, nullptr);
}

void udf_panel::Instance::tick(float deltaTime)
{
    tickAutohandle(deltaTime);

}

void udf_panel::Instance::end()
{
    endAutohandle();

}

udf_panel::Instance::~Instance()
{

}

void udf_panel::Instance::onEventConfigureStyle(ImGuiStyle& style, ImGuiIO& io)
{

}

#define ADD_YAML_VALUE(x, y, z) if (node[x]) \
    rhs.y = node[x].as<z>()

namespace YAML
{
    template<>
    struct convert<udf_panel::ExecModule>
    {
        static Node encode(const udf_panel::ExecModule&) noexcept
        {
            Node node;
            return node;
        }

        static bool decode(const Node& node, udf_panel::ExecModule& rhs) noexcept
        {
            if (node.IsSequence())
                return false;

            if (node["module"])
                rhs.name = node["module"].as<UImGui::FString>();

            ADD_YAML_VALUE("module", name, UImGui::FString);
            ADD_YAML_VALUE("command", command, UImGui::FString);
            ADD_YAML_VALUE("on-click", onClick, UImGui::FString);
            ADD_YAML_VALUE("on-double-click", onDoubleClick, UImGui::FString);
            ADD_YAML_VALUE("column", column, size_t);

            if (node["font"])
            {
                auto f = node["font"].as<UImGui::FString>();
                for (auto& a : static_cast<udf_panel::Instance*>(UImGui::Instance::getGlobal())->fonts)
                {
                    if (a.name == f)
                    {
                        rhs.font = &a;
                        break;
                    }
                }
            }
            return true;
        }
    };
}

void udf_panel::Instance::loadModules() noexcept
{
    YAML::Node out;
    try
    {
        out = YAML::LoadFile(UIMGUI_CONFIG_DIR"Dist/UDFPanelConfig.yaml");
    }
    catch (YAML::BadFile&)
    {
        Logger::log("Couldn't open the config file at: ", UVKLog::UVK_LOG_TYPE_ERROR, UIMGUI_CONFIG_DIR"Dist/UDFPanelConfig.yaml");
        std::terminate();
    }

    if (out["columns"])
        columns = out["columns"].as<int>();
    columns = columns == 0 ? 1 : columns;
    modules.resize(columns);

    if (out["top-margin"])
        topMargin = out["top-margin"].as<float>();
    if (out["position"])
        position = out["position"].as<UImGui::FVector2>();
    UImGui::Window::setCurrentWindowPosition(position);

    if (out["size"])
        size = out["size"].as<UImGui::FVector2>();
    if (size.x <= 0)
        size.x = 800;
    if (size.y <= 0)
        size.y = 32;
    UImGui::Window::setWindowSizeInScreenCoords(size);

    if (out["fonts"])
    {
        auto f = out["fonts"];
        for (const auto& font : f)
        {
            if (!font["file"])
                continue;
            fonts.push_back({ .name = font["font"].as<UImGui::FString>(), .location = font["file"].as<std::string>(), .size = font["size"].as<float>() });
        }
    }

    if (out["modules"])
    {
        auto f = out["modules"];
        for (const auto& module : f)
        {
            if (!module["type"])
                continue;
            if (UImGui::Utility::toLower(module["type"].as<UImGui::FString>().c_str()) == "exec")
            {
                execModules.push_back(module.as<ExecModule>());
                auto& back = execModules.back();

                // Set up columns
                back.column = back.column >= columns ? columns - 1 : back.column;

                float refreshAfter = -1;
                if (module["refresh-after"])
                    refreshAfter = module["refresh-after"].as<float>();

                // Add to modules as part of a column
                modules[back.column].push_back({ .data = &back, .type = UDF_PANEL_EXEC_MODULE, .refreshAfter = refreshAfter });
            }
            else
            {
                // TODO: Add plugin modules
            }
        }
    }
}