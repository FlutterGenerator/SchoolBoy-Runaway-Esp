#include <list>
#include <vector>
#include <string>
#include <string.h>
#include <pthread.h>
#include <cstring>
#include <jni.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include "Unity/Vector2.h"
#include "Unity/Vector3.h"
#include "Unity/Rect.h"
#include "Unity/Color.h"
#include "Unity/Quaternion.h"
#include "Includes/Logger.h"
#include "Includes/obfuscate.h"
#include "Includes/Utils.h"
#include "Includes/MonoString.h"
#include "Includes/Strings.h"

#include "KittyMemory/MemoryPatch.h"
#include "menu.h"

#include "ESP/hooks.h"
#include "ESP/Drawing.h"
#include "ESP/ESPManager.h"

#define targetLibName OBFUSCATE("libil2cpp.so")

ESPManager *espManager;
NepEsp es;

#include <Substrate/SubstrateHook.h>
#include <Substrate/CydiaSubstrate.h>

bool ESP, ESPLine, ESPBox;

float width = 1.0f;
Color color = Color::White();

struct My_Patches {
MemoryPatch godMode;
int setLinePosition = 0;
} patches;

void DrawESP(NepEsp esp, int screenWidth, int screenHeight) {

if(ESP) {
    
    if (espManager->enemies->empty()) {
        return;
    }
	
    for (int i = 0; i < espManager->enemies->size(); i++) {
        
        void *Player = (*espManager->enemies)[i]->object;
        if (PlayerAlive(Player)) {
        
        Rect rect;
        
        Vector3 PlayerPos = GetPlayerLocation(Player);
        
        void *Cam = get_camera();
        Vector3 PosNew = {0.f, 0.f, 0.f};
        PosNew = WorldToScreenPoint(Cam, PlayerPos);
        
        if (PosNew.z < 1.f) continue;
        
        Vector3 Origin;
        Origin = PlayerPos;
        Origin.y += 0.7f;
        float ps = 10 * 0.1;
        float ps2 = 10 * 0.1;
        Origin.y += ps;
        
        Vector3 BoxPosNew = {0.f, 0.f, 0.f};
        BoxPosNew = WorldToScreenPoint(Cam, Origin);
        
        float Hight =
        abs(BoxPosNew.y - PosNew.y) * (ps2 / ps), Width = Hight * 0.6f;
        
        rect = Rect(BoxPosNew.x - Width / 2.f,
        screenHeight - BoxPosNew.y,
        Width, Hight
        );
        
        Vector2 DrawFromTop = Vector2(screenWidth / 2, screenHeight / 0);
        Vector2 DrawFromCenter = Vector2(screenWidth / 2, screenHeight / 2);
        Vector2 DrawFromBottom = Vector2(screenWidth / 2, screenHeight / 1);
        Vector2 DrawTo = Vector2((screenWidth- (screenWidth - PosNew.x)) + 5, (screenHeight - PosNew.y - 10.0f));
        
        if (ESPLine){
        
        if (patches.setLinePosition == 0){
        esp.DrawLine(color, width, DrawFromTop, DrawTo);
        }
        if (patches.setLinePosition == 1) {
        esp.DrawLine(color, width, DrawFromCenter, DrawTo);
	    	}
        if (patches.setLinePosition == 2) {
        esp.DrawLine(color, width, DrawFromBottom, DrawTo);
		    }
        
        }
        
        if (ESPBox){
        esp.DrawBox(color , width, rect);
        }
        
        // ----- Offset to Fix Esp line Problem of your Screen
        /*
        auto Screen_SetResolution = (void (*)(int, int, bool)) (getAbsoluteAddress("libil2cpp.so",0x123456));
        Screen_SetResolution(screenWidth, screenHeight, true);
	    	*/
        
        } else {
            espManager->removeEnemyGivenObject(Player);
        }
    }

}
    
}

extern "C"
JNIEXPORT void JNICALL
Java_uk_lgl_modmenu_FloatingModMenuService_DrawOn(JNIEnv *env, jclass type, jobject espView, jobject canvas) {
                es = NepEsp(env, espView, canvas);
    if (es.isValid()){
        DrawESP(es, es.getWidth(), es.getHeight());
    }
    
 }

void *enemyPlayer = NULL;
void (*update)(void *player);
void _update(void *player) {
    if (player != NULL) {
        update(player);
        enemyPlayer = player;
    }
    if (ESP) {
        if (enemyPlayer) {
            espManager->tryAddEnemy(player);
        }
    }
}

/*
void (*ondestroy)(void *player);
void _ondestroy(void *player) {
   if (player != NULL) {
       ondestroy(player);
     espManager->removeEnemyGivenObject(player);
    }
}
*/

void *hack_thread(void *) {
    ProcMap il2cppMap;
    do {
        il2cppMap = KittyMemory::getLibraryMap("libil2cpp.so");
        sleep(1);
    } while (!isLibraryLoaded("libil2cpp.so"));
    espManager = new ESPManager();
    
    //Check if target lib is loaded
    do {
        sleep(1);
    } while (!isLibraryLoaded(targetLibName)); //dont forget to change this, if you change your lib

    // Offset you want Target object of your Esp public sealed class FaceActor : MonoBehaviour, IPreviewable private void LateUpdate() { }
    MSHookFunction((void *) getAbsoluteAddress("libil2cpp.so", 0x15B3340), (void *) &_update, (void **) &update);
    
    // Offset to Fix the Crash Problem
     //MSHookFunction((void *) getAbsoluteAddress("libil2cpp.so", 0xBF7028), (void *) &_ondestroy, (void **) &ondestroy);
	
    return NULL;
}

extern "C" {
    JNIEXPORT jobjectArray
    JNICALL
    Java_uk_lgl_modmenu_FloatingModMenuService_getFeatureList(JNIEnv *env, jobject context) {
        jobjectArray ret;
        const char *features[] = {
            
            "100_Toggle_Enable Esp",
            "200_Toggle_Esp Line",
            "300_Toggle_Esp Box",
            "400_RadioButton__Top,Center,Down",
            
        };
        int Total_Feature = (sizeof features / sizeof features[0]);
        ret = (jobjectArray)
        env->NewObjectArray(Total_Feature, env->FindClass(OBFUSCATE("java/lang/String")),
        env->NewStringUTF(""));

        for (int i = 0; i < Total_Feature; i++)
        env->SetObjectArrayElement(ret, i, env->NewStringUTF(features[i]));

        return (ret);
    }

    JNIEXPORT void JNICALL
    Java_uk_lgl_modmenu_Preferences_Changes(JNIEnv *env, jclass clazz, jobject obj,
        jint featNum, jstring featName, jint value,
        jboolean boolean, jstring str) {

        switch (featNum) {
        
        case 100:
        ESP = boolean;
        break;
        
        case 200:
        ESPLine = boolean;
        break;
        
        case 300:
        ESPBox = boolean;
        break;
        
        case 400:
        patches.setLinePosition = value;
        break;
		
        }
    }
}

__attribute__((constructor))
void lib_main() {

    pthread_t ptid;
    pthread_create(&ptid, NULL, hack_thread, NULL);
}

// Check platinmods.com site for more info about esp making.
