#include "Activity.h"

QJsonObject Activity::Timestamps::toJson() const {
    QJsonObject obj;

    if (start > 0) {
        obj["start"] = start;
    }

    if (end > 0) {
        obj["end"] = end;
    }

    return obj;
}

QJsonObject Activity::Assets::toJson() const {
    QJsonObject obj;

    if (!largeImage.isEmpty()) {
        obj["large_image"] = largeImage;
    }

    if (!largeText.isEmpty()) {
        obj["large_text"] = largeText;
    }

    if (!smallImage.isEmpty()) {
        obj["small_image"] = smallImage;
    }

    if (!smallText.isEmpty()) {
        obj["small_text"] = smallText;
    }

    return obj;
}

QJsonObject Activity::Button::toJson() const {
    return {
        {"label", label},
        {"url", url}
    };
}

QJsonObject Activity::toJson() const {
    QJsonObject obj;

    obj["type"] = static_cast<int>(type);

    if (!name.isEmpty()) {
        obj["name"] = name;
    }

    if (!state.isEmpty()) {
        obj["state"] = state;
    }

    if (!details.isEmpty()) {
        obj["details"] = details;
    }

    if (timestamps.start > 0 || timestamps.end > 0) {
        obj["timestamps"] = timestamps.toJson();
    }

    if (!assets.largeImage.isEmpty() || !assets.smallImage.isEmpty()) {
        obj["assets"] = assets.toJson();
    }

    if (!buttons.isEmpty()) {
        QJsonArray array;

        for (const auto &button : buttons) {
            array.append(button.toJson());
        }

        obj["buttons"] = array;
    }

    return obj;
}