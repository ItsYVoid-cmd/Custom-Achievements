#include <Geode/Geode.hpp>
#include <Geode/utils/file.hpp>
#include <Geode/loader/GameEvent.hpp>
#include <Geode/loader/SettingV3.hpp>

using namespace geode::prelude;

std::string iconFrame(std::string const& type, int id) {
    auto prefix =
        type == "ship" ? "ship_" :
        type == "ball" ? "player_ball_" :
        type == "ufo" ? "bird_" :
        type == "wave" ? "dart_" :
        type == "robot" ? "robot_" :
        type == "spider" ? "spider_" :
        type == "swing" ? "swing_" :
        type == "jetpack" ? "jetpack_" :
        "player_";

    return fmt::format("{}{:02}_001.png", prefix, id);
}

void createAchievementFile() {
    auto path = Mod::get()->getConfigDir() / "achievements.json";

    if (std::filesystem::exists(path))
        return;

    file::writeString(path, R"([
{
    "title": "Hello!",
    "icon": 10,
    "type": "swing"
},
{
    "title": "Second Achievement",
    "icon": 3,
    "type": "cube"
}
])");
}

$on_game(Loaded) {
    createAchievementFile();

    listenForKeybindSettingPresses(
        "trigger-next",
        [](Keybind const&, bool down, bool repeat, double) {
            if (!down || repeat)
                return;

            static size_t current = 0;

            auto result = file::readJson(
                Mod::get()->getConfigDir() / "achievements.json"
            );

            if (!result)
                return;

            auto json = result.unwrap();

            if (!json.isArray() || json.size() == 0)
                return;

            auto& achievement = json[current++ % json.size()];

            auto title =
                achievement.get<std::string>("title").unwrapOr("Achievement");

            auto icon =
                achievement.get<int>("icon").unwrapOr(1);

            auto type =
                achievement.get<std::string>("type").unwrapOr("cube");

            auto frame = iconFrame(type, icon);

            AchievementNotifier::sharedState()->notifyAchievement(
                title.c_str(),
                "",
                frame.c_str(),
                false
            );
        }
    );
}
