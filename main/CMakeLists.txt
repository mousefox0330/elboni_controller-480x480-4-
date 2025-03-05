#file(GLOB_RECURSE LV_DEMOS_SOURCES ./*.c)

idf_component_register(
    SRC_DIRS
        ${LV_DEMO_DIR}
        "."
		"middleware"
		"gui"
		"protocol"
		"modules"
		"bus"
    INCLUDE_DIRS
        ${LV_DEMO_DIR}
        "."
		"middleware"
		"gui"
		"protocol"
		"modules"
		"bus")
		
spiffs_create_partition_image(storage ../spiffs FLASH_IN_PROJECT)