#include "auton_selector.hpp"
#include "main.h"

namespace src::AutonSelector {
// Auton Selector Defaults and Variables
int autonIndexSelected = 0;
int AUTON_COUNT;                                                         // Set to actual number of autons
const char *autonNames[] = {"", "", "", "", "", "", "", "", "", "", ""}; // up to 10 autons

// Graphical Variables
lv_obj_t *tabView;
lv_obj_t *redButton;
lv_obj_t *blueButton;

// Auton Selector Functions
lv_res_t redButtonAction(lv_obj_t *button, const char *text) {
    // Find the index of the auton name
    for (int i = 0; i < AUTON_COUNT; i++) {
        if (strcmp(text, autonNames[i]) == 0) {
            autonIndexSelected = i + 1;
            // break;
        }
    }

    return LV_RES_OK; // return OK because the button matrix is not deleted
}

lv_res_t blueButtonAction(lv_obj_t *button, const char *text) {
    // Find the index of the auton name
    for (int i = 0; i < AUTON_COUNT; i++) {
        if (strcmp(text, autonNames[i]) == 0) {
            autonIndexSelected = -(i + 1);
            // break;
        }
    }

    return LV_RES_OK; // return OK because the button matrix is not deleted
}

lv_res_t skillsButtonAction(lv_obj_t *button) {
    autonIndexSelected = 0;
    return LV_RES_OK; // return OK because the button matrix is not deleted
}

void tabWatcherTask() {
    int activeTab = lv_tabview_get_tab_act(tabView);
    while (true) {
        // Get the current tab
        int currentTab = lv_tabview_get_tab_act(tabView);
        if (currentTab != activeTab) {
            activeTab = currentTab;
            if (activeTab == 0) {
                if (autonIndexSelected == 0)
                    autonIndexSelected = 1;
                autonIndexSelected = abs(autonIndexSelected);
                lv_btnm_set_toggle(redButton, true, abs(autonIndexSelected) - 1);
            } else if (activeTab == 1) {
                if (autonIndexSelected == 0)
                    autonIndexSelected = -1;
                autonIndexSelected = -abs(autonIndexSelected);
                lv_btnm_set_toggle(blueButton, true, abs(autonIndexSelected) - 1);
            } else {
                autonIndexSelected = 0;
            }
        }

        pros::delay(20);
    }
}

void initialize(const char **autons) {
    // Allocate some memory with some classic C
    int i = 0;
    do {
        memcpy(&autonNames[i], &autons[i], sizeof(&autons));
        i++;
    } while (strcmp(autons[i], "") != 0);

    AUTON_COUNT = i;
    autonIndexSelected = 1; // Default to No-op

    // lvgl theme
    lv_theme_t *th = lv_theme_alien_init(SELECTOR_HUE, NULL); // Set a HUE value and keep font default RED
    lv_theme_set_current(th);

    // Create a tab view object
    tabView = lv_tabview_create(lv_scr_act(), NULL);

    // add 3 tabs (the tabs are page (lv_page) and can be scrolled
    lv_obj_t *redTab = lv_tabview_add_tab(tabView, "Red");
    lv_obj_t *blueTab = lv_tabview_add_tab(tabView, "Blue");
    lv_obj_t *skillsTab = lv_tabview_add_tab(tabView, "Skills");

    // set default tab
    if (autonIndexSelected < 0) {
        lv_tabview_set_tab_act(tabView, 1, LV_ANIM_NONE);
    } else if (autonIndexSelected == 0) {
        lv_tabview_set_tab_act(tabView, 2, LV_ANIM_NONE);
    }

    // Create a button matrix on the red tab
    redButton = lv_btnm_create(redTab, NULL);
    lv_btnm_set_map(redButton, autonNames);
    lv_btnm_set_action(redButton, redButtonAction);
    lv_btnm_set_toggle(redButton, true, abs(autonIndexSelected) - 1);
    lv_obj_set_size(redButton, 450, 50);
    lv_obj_set_pos(redButton, 0, 100);
    lv_obj_align(redButton, NULL, LV_ALIGN_CENTER, 0, 0);

    // Create a button matrix on the blue tab
    blueButton = lv_btnm_create(blueTab, NULL);
    lv_btnm_set_map(blueButton, autonNames);
    lv_btnm_set_action(blueButton, blueButtonAction);
    lv_btnm_set_toggle(blueButton, true, abs(autonIndexSelected) - 1);
    lv_obj_set_size(blueButton, 450, 50);
    lv_obj_set_pos(blueButton, 0, 100);
    lv_obj_align(blueButton, NULL, LV_ALIGN_CENTER, 0, 0);

    // Create a button on the skills tab
    lv_obj_t *skillsButton = lv_btn_create(skillsTab, NULL);
    lv_obj_t *label = lv_label_create(skillsButton, NULL);
    lv_label_set_text(label, "Skills");
    lv_btn_set_action(skillsButton, LV_BTN_ACTION_CLICK, *skillsButtonAction);
    lv_obj_set_size(skillsButton, 450, 50);
    lv_obj_set_pos(skillsButton, 0, 100);
    lv_obj_align(skillsButton, NULL, LV_ALIGN_CENTER, 0, 0);

    // Create a task to watch for tab changes
    pros::Task tabWatcherHandle(tabWatcherTask);
}

} // namespace src::AutonSelector