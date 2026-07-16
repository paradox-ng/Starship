// Console GUI stub. The ImGui-based dev UI (src/port/ui) is excluded from the GX build
// (libultragx has no ImGui); the game core still calls into the GameUI namespace, so
// provide inert definitions to satisfy the link. There is no on-screen debug overlay.
namespace GameUI {
void SetupGuiElements() {}
void Destroy() {}
} // namespace GameUI
