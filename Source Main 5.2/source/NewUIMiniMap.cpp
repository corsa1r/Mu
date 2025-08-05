// NewUIGuildInfoWindow.cpp: implementation of the CNewUIGuildInfoWindow class.
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "NewUIMiniMap.h"
#include "NewUISystem.h"
#include "NewUICommonMessageBox.h"
#include "NewUICustomMessageBox.h"
#include "DSPlaySound.h"

#include "NewUIGuildInfoWindow.h"
#include "NewUIButton.h"
#include "NewUIMyInventory.h"
#include "CSitemOption.h"
#include "MapManager.h"

extern BYTE m_OccupationState;

using namespace SEASON3B;

SEASON3B::CNewUIMiniMap::CNewUIMiniMap()
{
    m_pNewUIMng = NULL;
}

SEASON3B::CNewUIMiniMap::~CNewUIMiniMap()
{
    Release();
}

bool SEASON3B::CNewUIMiniMap::Create(CNewUIManager* pNewUIMng, int x, int y)
{
    if (NULL == pNewUIMng)
        return false;

    m_pNewUIMng = pNewUIMng;
    m_pNewUIMng->AddUIObj(SEASON3B::INTERFACE_MINI_MAP, this);

    LoadBitmap(L"Interface\\mini_map_ui_corner.tga", IMAGE_MINIMAP_INTERFACE + 1, GL_LINEAR);
    LoadBitmap(L"Interface\\mini_map_ui_line.jpg", IMAGE_MINIMAP_INTERFACE + 2, GL_LINEAR);
    LoadBitmap(L"Interface\\mini_map_ui_cha.tga", IMAGE_MINIMAP_INTERFACE + 3, GL_LINEAR);
    LoadBitmap(L"Interface\\mini_map_ui_portal.tga", IMAGE_MINIMAP_INTERFACE + 4, GL_LINEAR);
    LoadBitmap(L"Interface\\mini_map_ui_npc.tga", IMAGE_MINIMAP_INTERFACE + 5, GL_LINEAR);
    LoadBitmap(L"Interface\\mini_map_ui_cancel.tga", IMAGE_MINIMAP_INTERFACE + 6, GL_LINEAR);

    m_BtnExit.ChangeButtonImgState(true, IMAGE_MINIMAP_INTERFACE + 6, false);
    m_BtnExit.ChangeButtonInfo(m_Pos.x + 610, 3, 85, 85);
    m_BtnExit.ChangeToolTipText(GlobalText[1002], true);

    SetPos(x, y);

    m_Lenth[0].x = 800;
    m_Lenth[0].y = 800;
    m_MiniPos = 0;
    m_bSuccess = false;
    return true;
}

void SEASON3B::CNewUIMiniMap::ClosingProcess()
{
    SocketClient->ToGameServer()->SendCloseNpcRequest();
}

float SEASON3B::CNewUIMiniMap::GetLayerDepth()
{
    return 8.1f;
}

void SEASON3B::CNewUIMiniMap::OpenningProcess()
{
}

void SEASON3B::CNewUIMiniMap::Release()
{
    UnloadImages();

    for (int i = 1; i < 7; i++)
    {
        DeleteBitmap(IMAGE_MINIMAP_INTERFACE + i);
    }

    if (m_pNewUIMng)
    {
        m_pNewUIMng->RemoveUIObj(this);
        m_pNewUIMng = NULL;
    }
}

void SEASON3B::CNewUIMiniMap::SetPos(int x, int y)
{
    m_BtnExit.ChangeButtonInfo(640 - 27, 3, 30, 25);
}

void SEASON3B::CNewUIMiniMap::SetBtnPos(int Num, float x, float y, float nx, float ny)
{
    m_Btn_Loc[Num][0] = x;
    m_Btn_Loc[Num][1] = y;
    m_Btn_Loc[Num][2] = nx;
    m_Btn_Loc[Num][3] = ny;
}

bool SEASON3B::CNewUIMiniMap::UpdateKeyEvent()
{
    return true;
    //if (g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_MINI_MAP))
    //{
    //    if (IsPress(VK_ESCAPE) == true || IsPress(VK_TAB) == true)
    //    {
    //        g_pNewUISystem->Hide(SEASON3B::INTERFACE_MINI_MAP);
    //        PlayBuffer(SOUND_CLICK01);
    //        return false;
    //    }
    //}
}

bool SEASON3B::CNewUIMiniMap::Render()
{
    float Rot = 0.0f;

    if (m_bSuccess == false)
        return m_bSuccess;

    EnableAlphaTest();
    const float zoomScale = 0.68f;
    const float uiMapWidth = 120;
    const float uiMapHeight = 90;
    float screenWidth = 1920; // Get from config / opengl
    float screenHeight = 1080; // Get from config / opengl
    float screenMapWidth = 400;
    float screenMapHeight = 200;
    float uiScreenWidth = 640;
    float uiScreenHeight = 430;
    float uiScreenRatioX = screenWidth / uiScreenWidth;
    float uiScreenRatioY = screenHeight / uiScreenHeight;

    RenderColor(
        uiScreenWidth - uiMapWidth - 3,
        0,
        uiMapWidth + 10,
        uiMapHeight + 10,
        0.85f,
        1
    );

    // Enable scissor test for scaled area
    glEnable(GL_SCISSOR_TEST);
    glScissor(
        (uiScreenWidth - uiMapWidth - 2) * uiScreenRatioX,
        (uiScreenHeight - uiMapHeight + 2) * uiScreenRatioY,
        uiMapWidth * uiScreenRatioX,
        uiMapHeight * uiScreenRatioY
    );

    DisableAlphaBlend();
    EnableAlphaTest();
    glColor4f(1.f, 1.f, 1.f, 1.f);

    // Scale position calculations
    auto Ty = (float)(((float)Hero->PositionX / 256.f) * m_Lenth[m_MiniPos].y * zoomScale);
    auto Tx = (float)(((float)Hero->PositionY / 256.f) * m_Lenth[m_MiniPos].x * zoomScale);
    float Ty1;
    float Tx1;
    // UV coordinates remain relative to texture
    float uvxy = (41.7f / 64.f);
    float uvxy_Line = 8.f / 8.f;
    // Scale UI dimensions: 35 * zoomScale ≈ 12.3046875, 6 * zoomScale ≈ 2.109375
    float Ui_wid = 12.3046875f;
    float Ui_Hig = 2.109375f;
    float Rot_Loc = 45.0f;
    int i = 0;

    RenderBitRotate(
        IMAGE_MINIMAP_INTERFACE, 
        m_Lenth[m_MiniPos].x - Tx, 
        m_Lenth[m_MiniPos].y - Ty - uiScreenHeight - 35,
        m_Lenth[m_MiniPos].x * zoomScale, 
        m_Lenth[m_MiniPos].y * zoomScale,
        Rot
    );

    // Scale NPC marker sizes: 10 * 1.5 = 15, 15 * 1.5 = 22.5   
    //int NpcWidth = 15;
    //int NpcWidthP = 22.5;
    //for (i = 0; i < MAX_MINI_MAP_DATA; i++)
    //{
    //    if (m_Mini_Map_Data[i].Kind <= 0) continue;

    //    // Scale NPC positions
    //    Ty1 = (float)(((float)m_Mini_Map_Data[i].Location[0] / 256.f) * m_Lenth[m_MiniPos].y * zoomScale);
    //    Tx1 = (float)(((float)m_Mini_Map_Data[i].Location[1] / 256.f) * m_Lenth[m_MiniPos].x * zoomScale);
    //    Rot_Loc = (float)m_Mini_Map_Data[i].Rotation;

    //    if (m_Mini_Map_Data[i].Kind == 1) //npc
    //    {
    //        if (!(gMapManager.WorldActive == WD_34CRYWOLF_1ST && m_OccupationState > 0) ||
    //            (m_Mini_Map_Data[i].Location[0] == 228 && m_Mini_Map_Data[i].Location[1] == 48 && gMapManager.WorldActive == WD_34CRYWOLF_1ST))
    //            RenderPointRotate(IMAGE_MINIMAP_INTERFACE + 5,
    //                Tx1, Ty1, NpcWidth, NpcWidth,
    //                m_Lenth[m_MiniPos].x * zoomScale - Tx,
    //                m_Lenth[m_MiniPos].y * zoomScale - Ty,
    //                m_Lenth[m_MiniPos].x * zoomScale,
    //                m_Lenth[m_MiniPos].y * zoomScale,
    //                Rot, Rot_Loc, 17.5f / 32.f, 17.5f / 32.f, i);
    //    }
    //    else if (m_Mini_Map_Data[i].Kind == 2)
    //    {
    //        RenderPointRotate(IMAGE_MINIMAP_INTERFACE + 4,
    //            Tx1, Ty1, NpcWidthP, NpcWidthP,
    //            m_Lenth[m_MiniPos].x * zoomScale - Tx,
    //            m_Lenth[m_MiniPos].y * zoomScale - Ty,
    //            m_Lenth[m_MiniPos].x * zoomScale,
    //            m_Lenth[m_MiniPos].y * zoomScale,
    //            Rot, Rot_Loc, 17.5f / 32.f, 17.5f / 32.f, 100 + i);
    //    }
    //}

    // Scale character marker: 4 * 1.5 = 6
    float Ch_wid = 6;
    // Center character marker in 225x225 area: (325, 230) remains unscaled for fixed UI position
    RenderImage(
        IMAGE_MINIMAP_INTERFACE + 3, 
        (uiScreenWidth) - (uiMapWidth / 2) - (Ch_wid / 2),
        (uiMapHeight / 2) - (Ch_wid / 2),
        Ch_wid, 
        Ch_wid, 
        0.f, 
        0.f, 
        17.5f / 32.f, 
        17.5f / 32.f
    );

    // Uncomment and scale borders/corners if needed
    /*
    for (i = 0; i < 25; i++)
    {
        RenderImage(IMAGE_MINIMAP_INTERFACE + 2, i * Ui_wid, 0, Ui_wid, Ui_Hig, 0.f, 1.f, uvxy, -uvxy_Line);
        RenderImage(IMAGE_MINIMAP_INTERFACE + 2, i * Ui_wid, mapHeight - Ui_Hig, Ui_wid, Ui_Hig, 0.f, 0.f, uvxy, uvxy_Line);
    }
    for (i = 0; i < 20; i++)
    {
        RenderBitmapRotate(IMAGE_MINIMAP_INTERFACE + 2, 1.0546875f, i * (Ui_wid - 1.7578125f), Ui_wid, Ui_Hig, -90.f, 0.f, 0.f, uvxy, uvxy_Line);
        RenderBitmapRotate(IMAGE_MINIMAP_INTERFACE + 2, mapWidth - 1.0546875f, i * (Ui_wid - 1.7578125f), Ui_wid, Ui_Hig, 90.f, 0.f, 0.f, uvxy, uvxy_Line);
    }
    RenderImage(IMAGE_MINIMAP_INTERFACE + 1, 0, 0, Ui_wid, Ui_wid, 0.f, 0.f, uvxy, uvxy);
    RenderImage(IMAGE_MINIMAP_INTERFACE + 1, mapWidth - Ui_wid, 0, Ui_wid, Ui_wid, uvxy, 0.f, -uvxy, uvxy);
    RenderImage(IMAGE_MINIMAP_INTERFACE + 1, 0, mapHeight - Ui_wid, Ui_wid, Ui_wid, 0.f, uvxy, uvxy, -uvxy);
    RenderImage(IMAGE_MINIMAP_INTERFACE + 1, mapWidth - Ui_wid, mapHeight - Ui_wid, Ui_wid, Ui_wid, uvxy, uvxy, -uvxy, -uvxy);
    */

    //m_BtnExit.Render(true);

    glDisable(GL_SCISSOR_TEST);
    DisableAlphaBlend();

    // TODO this should also check if within cut box
    // Check_Btn(MouseX, MouseY); 
    return true;
}

bool SEASON3B::CNewUIMiniMap::Update()
{
    return true;
}

void SEASON3B::CNewUIMiniMap::LoadImages(const wchar_t* Filename)
{
    wchar_t Fname[300];
    int i = 0;
    swprintf(Fname, L"Data\\%s\\mini_map.ozt", Filename);
    FILE* pFile = _wfopen(Fname, L"rb");

    if (pFile == NULL)
    {
        m_bSuccess = false;
        return;
    }
    else
    {
        m_bSuccess = true;
        fclose(pFile);
        swprintf(Fname, L"%s\\mini_map.tga", Filename);
        LoadBitmap(Fname, IMAGE_MINIMAP_INTERFACE, GL_LINEAR);
    }

    swprintf(Fname, L"Data\\Local\\%s\\Minimap\\Minimap_%s_%s.bmd", g_strSelectedML.c_str(), Filename, g_strSelectedML.c_str());

    for (i = 0; i < MAX_MINI_MAP_DATA; i++)
    {
        m_Mini_Map_Data[i].Kind = 0;
    }

    FILE* fp = _wfopen(Fname, L"rb");

    if (fp != NULL)
    {
        int Size = sizeof(MINI_MAP_FILE);
        BYTE* Buffer = new BYTE[Size * MAX_MINI_MAP_DATA + 45];
        fread(Buffer, (Size * MAX_MINI_MAP_DATA) + 45, 1, fp);

        DWORD dwCheckSum;
        fread(&dwCheckSum, sizeof(DWORD), 1, fp);
        fclose(fp);

        if (dwCheckSum != GenerateCheckSum2(Buffer, (Size * MAX_MINI_MAP_DATA) + 45, 0x2BC1))
        {
            wchar_t Text[256];
            swprintf(Text, L"%s - File corrupted.", Fname);
            g_ErrorReport.Write(Text);
            MessageBox(g_hWnd, Text, NULL, MB_OK);
            SendMessage(g_hWnd, WM_DESTROY, 0, 0);
        }
        else
        {
            BYTE* pSeek = Buffer;

            for (i = 0; i < MAX_MINI_MAP_DATA; i++)
            {
                BuxConvert(pSeek, Size);
                //memcpy(&(m_Mini_Map_Data[i]), pSeek, Size);

                MINI_MAP_FILE current{ };
                auto target = &(m_Mini_Map_Data[i]);
                memcpy(&current, pSeek, Size);
                memcpy(target, pSeek, Size);

                CMultiLanguage::ConvertFromUtf8(target->Name, current.Name);
                /*int wchars_num = MultiByteToWideChar(CP_UTF8, 0, current.Name, -1, NULL, 0);
                MultiByteToWideChar(CP_UTF8, 0, current.Name, -1, target->Name, wchars_num);
                target->Name[wchars_num] = L'\0';*/
                pSeek += Size;
            }
        }

        delete[] Buffer;
    }
}

void SEASON3B::CNewUIMiniMap::UnloadImages()
{
    DeleteBitmap(IMAGE_MINIMAP_INTERFACE);
}

bool SEASON3B::CNewUIMiniMap::UpdateMouseEvent()
{
    return true;
    //bool ret = true;

    //if (m_BtnExit.UpdateMouseEvent() == true)
    //{
    //    g_pNewUISystem->Hide(SEASON3B::INTERFACE_MINI_MAP);
    //    return true;
    //}

    //if (IsPress(VK_LBUTTON))
    //{
    //    ret = Check_Mouse(MouseX, MouseY);
    //    if (ret == false)
    //    {
    //        PlayBuffer(SOUND_CLICK01);
    //    }
    //}

    //if (CheckMouseIn(0, 0, 640, 430))
    //{
    //    return false;
    //}

    //return ret;
}

bool SEASON3B::CNewUIMiniMap::Check_Mouse(int mx, int my)
{
    return false;
}

bool SEASON3B::CNewUIMiniMap::Check_Btn(int mx, int my)
{
    int i = 0;
    for (i = 0; i < MAX_MINI_MAP_DATA; i++)
    {
        if (m_Mini_Map_Data[i].Kind > 0)
        {
            if (mx > m_Btn_Loc[i][0] && mx < (m_Btn_Loc[i][0] + m_Btn_Loc[i][2]) && my > m_Btn_Loc[i][1] && my < (m_Btn_Loc[i][1] + m_Btn_Loc[i][3]))
            {
                SIZE Fontsize;
                m_TooltipText = (std::wstring)m_Mini_Map_Data[i].Name;
                g_pRenderText->SetFont(g_hFont);
                GetTextExtentPoint32(g_pRenderText->GetFontDC(), m_TooltipText.c_str(), m_TooltipText.size(), &Fontsize);

                Fontsize.cx = Fontsize.cx / ((float)WindowWidth / 640);
                Fontsize.cy = Fontsize.cy / ((float)WindowHeight / 480);

                int x = m_Btn_Loc[i][0] + ((m_Btn_Loc[i][2] / 2) - (Fontsize.cx / 2));
                int y = m_Btn_Loc[i][1] + m_Btn_Loc[i][3] + 2;

                y = m_Btn_Loc[i][1] - (Fontsize.cy + 2);

                DWORD backuptextcolor = g_pRenderText->GetTextColor();
                DWORD backuptextbackcolor = g_pRenderText->GetBgColor();

                g_pRenderText->SetTextColor(RGBA(255, 255, 255, 255));
                g_pRenderText->SetBgColor(RGBA(0, 0, 0, 180));
                g_pRenderText->RenderText(x, y, m_TooltipText.c_str(), Fontsize.cx + 6, 0, RT3_SORT_CENTER);

                g_pRenderText->SetTextColor(backuptextcolor);
                g_pRenderText->SetBgColor(backuptextbackcolor);

                return true;
            }
        }
        else
            break;
    }
    return false;
}