#ifndef CANVASITEM_H
#define CANVASITEM_H

#include <QRect>
#include <QColor>
#include <QString>

// ─────────────────────────────────────────────────────────────────────
// WidgetType — the vocabulary shared by the entire application.
// Every file that needs to know "what kind of widget is this?"
// includes canvasitem.h and uses this enum.
// ─────────────────────────────────────────────────────────────────────
enum class WidgetType {
    Button,
    Label,
    TextField,
    Checkbox,
    RadioButton,
    Unknown
};

// ─────────────────────────────────────────────────────────────────────
// CanvasItem — the MODEL.
// A plain C++ struct with no Qt parent, no painting code, no event
// code. Pure data. Stored by VALUE in QList<CanvasItem>.
// ─────────────────────────────────────────────────────────────────────
struct CanvasItem {
    WidgetType type     = WidgetType::Unknown;
    QRect      rect;                            // position + size in CANVAS coordinates
    QColor     color;                           // fill color
    QString    label;                           // display text
    bool       selected = false;                // is this item currently selected?
};

// ─────────────────────────────────────────────────────────────────────
// Inline helpers — live in the header so every includer gets them
// without needing a separate .cpp
// ─────────────────────────────────────────────────────────────────────

inline WidgetType widgetTypeFromString(const QString &s)
{
    if (s == "Button")      return WidgetType::Button;
    if (s == "Label")       return WidgetType::Label;
    if (s == "TextField")   return WidgetType::TextField;
    if (s == "Checkbox")    return WidgetType::Checkbox;
    if (s == "RadioButton") return WidgetType::RadioButton;
    return WidgetType::Unknown;
}

inline QString labelForType(WidgetType t)
{
    switch (t) {
    case WidgetType::Button:      return "Button";
    case WidgetType::Label:       return "Label";
    case WidgetType::TextField:   return "TextField";
    case WidgetType::Checkbox:    return "Checkbox";
    case WidgetType::RadioButton: return "RadioButton";
    default:                      return "Widget";
    }
}

inline QColor defaultColorForType(WidgetType t)
{
    switch (t) {
    case WidgetType::Button:      return QColor(100, 149, 237, 210); // cornflower blue
    case WidgetType::Label:       return QColor(240, 128, 128, 210); // light coral
    case WidgetType::TextField:   return QColor(144, 238, 144, 210); // light green
    case WidgetType::Checkbox:    return QColor(255, 215,   0, 210); // gold
    case WidgetType::RadioButton: return QColor(221, 160, 221, 210); // plum
    default:                      return QColor(200, 200, 200, 210); // gray
    }
}

#endif // CANVASITEM_H