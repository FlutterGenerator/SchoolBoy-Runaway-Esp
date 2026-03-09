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
#include <GLES3/gl3.h>
#include <GLES3/gl31.h>
#include <GLES3/gl32.h>
#include "Unity/Vector2.h"
#include "Unity/Vector3.h"
#include "Unity/Rect.h"
#include "Unity/Color.h"
#include "Unity/Quaternion.h"
#include "Includes/Logger.h"
#include "Includes/obfuscate.h"
#include "Includes/Utils.h"
#include "Includes/Chams.h"
#include "Includes/MonoString.h"
#include "Includes/Strings.h"
#include "KittyMemory/MemoryPatch.h"

#include "Menu.h"

#include "ESP/Hooks.h"
#include "ESP/Drawing.h"
#include "ESP/ESPManager.h"

//Target lib here
#define targetLibName OBFUSCATE("libil2cpp.so")

ESPManager *espManager;
NepEsp es;

#include <Substrate/SubstrateHook.h>
#include <Substrate/CydiaSubstrate.h>

bool ESP, ESPLine, ESPBox;
bool chams, wireframe, glow, outline, rainbow = false;
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
    } while (!il2cppMap.isValid() && mlovinit());
    espManager = new ESPManager();
    setShader("_OFF");
    LogShaders();
    Wallhack();

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
            OBFUSCATE("Collapse_Esp Menu"), //Not Counted
            OBFUSCATE("1_CollapseAdd_Toggle_Enable Esp"), //1 Case
            OBFUSCATE("2_CollapseAdd_Toggle_Esp Line"), //2 Case
            OBFUSCATE("3_CollapseAdd_Toggle_Esp Box"), //3 Case
            OBFUSCATE("4_CollapseAdd_RadioButton__Top,Center,Down"), //4 Case
            OBFUSCATE("Collapse_Chams Menu"), //Not Counted
            OBFUSCATE("5_CollapseAdd_CheckBox_Default Chams"), //5 Case
            OBFUSCATE("6_CollapseAdd_CheckBox_Wireframe Chams"), //6 Case
            OBFUSCATE("7_CollapseAdd_CheckBox_Glow Chams"), //7 Case
            OBFUSCATE("8_CollapseAdd_CheckBox_Outline Chams"), //8 Case
            OBFUSCATE("9_CollapseAdd_CheckBox_Rainbow Chams"), //9 Case
            OBFUSCATE("10_CollapseAdd_SeekBar_Line Width_0_10"), //10 Case
            OBFUSCATE("11_CollapseAdd_SeekBar_Color Red_0_255"), //11 Case
            OBFUSCATE("12_CollapseAdd_SeekBar_Color Green_0_255"), //12 Case
            OBFUSCATE("13_CollapseAdd_SeekBar_Color Blue_0_255"), //13 Case
            OBFUSCATE("14_CollapseAdd_Spinner_Select Chams Shader_None (Default),Shader 1,Shader 2,Shader 3,Shader 4,Shader 5,Shader 6,Shader 7,Shader 8,Shader 9,Shader 10,Shader 11,Shader 12,Shader 13,Shader 14,Shader 15, Shader 16, Shader 17, Shader 18, Shader 19"),
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

        case 1:
            ESP = boolean;
            break;

        case 2:
            ESPLine = boolean;
            break;

        case 3:
            ESPBox = boolean;
            break;

        case 4:
            patches.setLinePosition = value;
            break;

        case 5:
            chams = boolean;
            if (chams) {
                SetWallhack(boolean);
            }
            break;

        case 6:
            wireframe = boolean;
            if (wireframe) {
                SetWallhackW(boolean);
            }
            break;

        case 7:
            glow = boolean;
            if (glow) {
                SetWallhackG(boolean);
            }
            break;

        case 8:
            outline = boolean;
            if (outline) {
                SetWallhackO(boolean);
            }
            break;

        case 9:
            rainbow = boolean;
            if (rainbow) {
                SetRainbow1(boolean);
            }
            break;

        case 10:
            SetW(value);
            break;

        case 11:
            SetR(value);
            break;

        case 12:
            SetG(value);
            break;

        case 13:
            SetB(value);
            break;

        case 14:
            switch (value) {
                case 0:
                    setShader("Off");
                    break;
                case 1:
                    setShader("_MainTex");
                    break;
                case 2:
                    setShader("_MainTex_ST");
                    break;
                case 3:
                    setShader("hlslcc_mtx4x4unity_MatrixVP[0]");
                    break;
                case 4:
                    setShader("hlslcc_mtx4x4unity_WorldToObject[0]");
                    break;
                case 5:
                    setShader("hlslcc_mtx4x4unity_ObjectToWorld[0]");
                    break;
                case 6:
                    setShader("_Color");
                    break;
                case 7:
                    setShader("_MainTex_ST");
                    break;
                case 8:
                    setShader("hlslcc_mtx4x4unity_MatrixVP[0]");
                    break;
                case 9:
                    setShader("hlslcc_mtx4x4unity_WorldToObject[0]");
                    break;
                case 10:
                    setShader("_WorldSpaceCameraPos[0]");
                    break;
                case 11:
                    setShader("unity_FogParams");
                    break;
                case 12:
                    setShader("hlslcc_mtx4x4unity_MatrixV[0]");
                    break;
                case 13:
                    setShader("_EmisColor");
                    break;
                case 14:
                    setShader("_Cube");
                    break;
                case 15:
                    setShader("_BumpMap");
                    break;
                case 16:
                    setShader("unity_WorldTransformParams");
                    break;
                case 17:
                    setShader("_Shininess");
                    break;
                case 18:
                    setShader("unity_Lightmap");
                    break;
                case 19:
                    setShader("_MainTex2_ST");
                    break;
            }
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
