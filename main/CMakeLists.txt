#file(GLOB_RECURSE LV_DEMOS_SOURCES ./*.c)

idf_component_register(
    SRC_DIRS
        ${LV_DEMO_DIR}
        "."
		"middleware"
		"gui"
		"gui/assert"
		"gui/font"
		"protocol"
		"modules"
		"bus"
    INCLUDE_DIRS
        ${LV_DEMO_DIR}
        "."
		"middleware"
		"gui"
		"gui/assert"
		"gui/font"
		"protocol"
		"modules"
		"bus")

add_compile_definitions(LV_LVGL_H_INCLUDE_SIMPLE)

spiffs_create_partition_image(storage ../spiffs FLASH_IN_PROJECT)