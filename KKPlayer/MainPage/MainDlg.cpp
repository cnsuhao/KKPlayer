// MainDlg.cpp : implementation of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MainDlg.h"
#include <dwmapi.h>
#include <atldlgs.h>
#include "SuiVideo.h"
#include "../Tool/cchinesecode.h"
#include <control/SCmnCtrl.h>
#include <control/SSliderBar.h>
#include <control/SListView.h>
#include <queue>
#include <map>

extern SOUI::CAutoRefPtr<SOUI::IRenderFactory> pRenderFactory;
#include <helper/SAdapterBase.h>
#pragma comment(lib,"dwmapi.lib")
namespace SOUI
{


	class CTestAdapterFix : public SAdapterBase
	{
		int * m_pCbxSel;
		CSuiVideo *m_pSuiVideo;
		std::vector<AV_Hos_Info *> m_slQue;
		std::map<int,IBitmap*> m_BitMap;
		CMainDlg *m_pCMainDlg;
	public:

		CTestAdapterFix(CMainDlg *pCMainDlg,CSuiVideo *pSuiVideo)
		{
			m_pCMainDlg=pCMainDlg;
			m_pSuiVideo=pSuiVideo;
			m_pSuiVideo->GetAVHistoryInfo(m_slQue);
			int count=m_slQue.size();
			if(count>0)
			{
				m_pCbxSel = new int[count];
				memset(m_pCbxSel,0,sizeof(int)*count);
			}		
		}

		~CTestAdapterFix()
		{
			delete []m_pCbxSel;   

			for(int i=0;i<m_slQue.size();i++)
			{
				AV_Hos_Info * p=m_slQue.at(i);
				free(p->pBuffer);
				free(p);
			}
			m_slQue.clear();
		}

		virtual int getCount()
		{
			return m_slQue.size();
		}   

		bool OnButtonClick(EventArgs *pEvt)
		{
			
			return true;
		}
		virtual void getView(int position, SWindow * pItem,pugi::xml_node xmlTemplate)
		{
					AV_Hos_Info * pAVPic=m_slQue.at(position);
					if(pItem->GetChildrenCount()==0)
					{
						pItem->InitFromXml(xmlTemplate);
					}

					SItemPanel * pSItme=(SItemPanel *)pItem;
					pSItme->SetUserData((ULONG_PTR)pAVPic);
					pSItme->GetEventSet()->subscribeEvent(EVT_ITEMPANEL_DBCLICK,Subscriber(&CTestAdapterFix::OnCbxSelChange,this));
					SImageWnd  *pAV_img= pItem->FindChildByName2<SImageWnd>(L"AV_img");
					std::map<int,IBitmap*>::iterator _It=m_BitMap.find(position);
					if(_It==m_BitMap.end())
					{
						BITMAPINFOHEADER header;
						header.biSize = sizeof(BITMAPINFOHEADER);
						int bpp=32;
						header.biWidth = pAVPic->width;
						header.biHeight = pAVPic->height*(-1);
						header.biBitCount = bpp;
						header.biCompression = 0;
						header.biSizeImage = 0;
						header.biClrImportant = 0;
						header.biClrUsed = 0;
						header.biXPelsPerMeter = 0;
						header.biYPelsPerMeter = 0;
						header.biPlanes = 1;


						//3 构造文件头
						BITMAPFILEHEADER bmpFileHeader;
						HANDLE hFile = NULL;
						DWORD dwTotalWriten = 0;
						DWORD dwWriten;

						bmpFileHeader.bfType = 0x4d42; //'BM';
						bmpFileHeader.bfOffBits=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);
						bmpFileHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)+ pAVPic->width*pAVPic->height*bpp/8;


						int Totl=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+pAVPic->bufLen;
						unsigned char *bufx=(unsigned char *)::malloc(Totl);
						unsigned char *bufx2=bufx;
						memcpy(bufx2,&bmpFileHeader, sizeof(BITMAPFILEHEADER));

						bufx2+=sizeof(BITMAPFILEHEADER);

						memcpy(bufx2,&header, sizeof(BITMAPINFOHEADER));
						bufx2+=sizeof(BITMAPINFOHEADER);
						memcpy(bufx2,pAVPic->pBuffer, pAVPic->bufLen);
						IBitmap *pImg=NULL;
						pRenderFactory->CreateBitmap(&pImg);
						HRESULT  ll=0;//pImg->LoadFromFile(L"D:/pic/0.bmp");

						ll=pImg->LoadFromMemory(bufx,Totl);
						/*IBitmap *pImg = new IBitmap();*/

						pAV_img->SetImage(pImg);

						m_BitMap.insert(std::pair<int,IBitmap*>(position,pImg));
						::free(bufx);
					}else
					{
						 pAV_img->SetImage(_It->second);
					}
					
					
                    wchar_t strtmp[256]=L"";
					std::wstring CurTimeStr;
					CChineseCode::charTowchar(pAVPic->path,strtmp,256);
					CurTimeStr=strtmp;
					int index=CurTimeStr.find_last_of(L"\\");
					if(index<0)
						index=CurTimeStr.find_last_of(L"//");
					if(index>=0)
					{
						 index++;
                         CurTimeStr=CurTimeStr.substr(index,CurTimeStr.length()-index);
						 index=CurTimeStr.find_last_of(L".");
                         CurTimeStr=CurTimeStr.substr(0,index);
					}
					

					SStatic *pTxt = pItem->FindChildByName2<SStatic>(L"AV_name");
					//pTxt->SetUserData();
					pTxt->GetUserData();
					pTxt->SetWindowText(CurTimeStr.c_str());



					SStatic *pAV_CurTime= pItem->FindChildByName2<SStatic>(L"AV_CurTime");
					

					int h=(pAVPic->CurTime/(60*60));
					int m=(pAVPic->CurTime%(60*60))/(60);
					int s=((pAVPic->CurTime%(60*60))%(60));
					
					if(h<10){
						swprintf_s(strtmp,L"0%d:",h);
						CurTimeStr=strtmp;
					}
					else{
						swprintf_s(strtmp,L"%d:",h);
						CurTimeStr=strtmp;
					}
					if(m<10){
						swprintf_s(strtmp,L"0%d:",m);
						CurTimeStr+=strtmp;
					}
					else{				  
						swprintf_s(strtmp,L"%d:",m);
						CurTimeStr+=strtmp;
					}

					if(s<10){
						swprintf_s(strtmp,L"0%d",s);
						CurTimeStr+=strtmp;
					}
					else{
						swprintf_s(strtmp,L"%d",s);
						CurTimeStr+=strtmp;
					}
					pAV_CurTime->SetWindowText(CurTimeStr.c_str());
			

					h=pAVPic->TotalTime/(60*60);
					m=(pAVPic->TotalTime%(60*60))/60;
					s=((pAVPic->TotalTime%(60*60))%60);


					if(h<10)
					{
						swprintf_s(strtmp,L"0%d:",h);
						CurTimeStr=strtmp;
					}
					else{
						swprintf_s(strtmp,L"%d:",m);
						CurTimeStr=strtmp;
					}
					if(m<10){
						swprintf_s(strtmp,L"0%d:",m);
						CurTimeStr+=strtmp;
					}
					else{
						swprintf_s(strtmp,L"%d:",m);
						CurTimeStr+=strtmp;
					}
					if(s<10){
						swprintf_s(strtmp,L"0%d",s);
						CurTimeStr+=strtmp;
					}
					else{
						swprintf_s(strtmp,L"%d",s);
						CurTimeStr+=strtmp;
					}

					SStatic *pAV_Time= pItem->FindChildByName2<SStatic>(L"AV_Time");
					pAV_Time->SetWindowText(CurTimeStr.c_str());
					
          
		}

		bool OnCbxSelChange(EventArgs *pEvt)
		{
			SItemPanel * pSItme=(SItemPanel *)pEvt->sender;

			ULONG_PTR p=pSItme->GetUserData();
			AV_Hos_Info * pAVPic=(AV_Hos_Info *)p;
			m_pCMainDlg->OpenMedia(pAVPic->path);
			return true;
		}

		
	};


	CMainDlg::CMainDlg() : SHostWnd(_T("LAYOUT:XML_MAINWND"))
	{
		m_bLayoutInited=FALSE;
		m_PlayerState=0;
		m_lastSeekTime=0;
		m_MenuId=0;
		m_bFullScreen=false;
		m_bScreenNormal=true;
	} 

	CMainDlg::~CMainDlg()
	{
	}

	int CMainDlg::OnCreate( LPCREATESTRUCT lpCreateStruct )
	{
		// 		MARGINS mar = {5,5,30,5};
		// 		DwmExtendFrameIntoClientArea ( m_hWnd, &mar );
		SetMsgHandled(FALSE);
		
		
		return 0;
	}

	void CMainDlg::OnShowWindow( BOOL bShow, UINT nStatus )
	{
		if(bShow)
		{
 			//AnimateHostWindow(200,AW_CENTER);
		}
	}


	BOOL CMainDlg::OnInitDialog( HWND hWnd, LPARAM lParam )
	{
		m_bLayoutInited=TRUE;

        SetMsgHandled(FALSE);
		CSuiVideo* pAV=(CSuiVideo* )FindChildByName(L"AVMainWnd");
		

		//行高固定的列表
		SListView *pLstViewFix = FindChildByName2<SListView>("AV_list");
		if(pLstViewFix)
		{
			ILvAdapter *pAdapter = new CTestAdapterFix(this,pAV);
			pLstViewFix->SetAdapter(pAdapter);
			pAdapter->Release();
				// pAV_img->GetEventSet()->subscribeEvent(EVT_CMD,Subscriber(&CTestAdapterFix::OnButtonClick,this));
		}

		return 0;
	}

	void CMainDlg::OnSize(UINT nType, CSize size)
	{
		SetMsgHandled(FALSE);
		if(!m_bLayoutInited)
			return;
		if(nType==SIZE_MAXIMIZED)
		{
			FindChildByName(L"btn_restore")->SetVisible(TRUE);
			FindChildByName(L"btn_max")->SetVisible(FALSE);
		}else if(nType==SIZE_RESTORED)
		{
			FindChildByName(L"btn_restore")->SetVisible(FALSE);
			FindChildByName(L"btn_max")->SetVisible(TRUE);
		}
	}

	void CMainDlg::OnBtnMsgBox()
	{
		SMessageBox(NULL,_T("this is a message box"),_T("haha"),MB_OK|MB_ICONEXCLAMATION);
		SMessageBox(NULL,_T("this message box includes two buttons"),_T("haha"),MB_YESNO|MB_ICONQUESTION);
		SMessageBox(NULL,_T("this message box includes three buttons"),NULL,MB_ABORTRETRYIGNORE);
	}
	void CMainDlg::OnAVPlay()
	{
		SImageButton* pPlay=(SImageButton*)FindChildByName(L"AVPlayBtn");
		CSuiVideo* av=(CSuiVideo*)FindChildByName(L"AVMainWnd");
		if(m_PlayerState==1){
            pPlay->SetAttribute(L"skin",L"_skin.PLAY",TRUE);
			av->SetPlayStat(1);
			pPlay->UpdateWindow();
			m_PlayerState=2;
		}else if(m_PlayerState==2){
                pPlay->SetAttribute(L"skin",L"_skin.Pause",TRUE);
				av->SetPlayStat(1);
				m_PlayerState=1;
				pPlay->UpdateWindow();
		}

	}
	void CMainDlg::OnTimer(char cTimerID)
	{
        if(m_PlayerState!=0)
		{
             CSuiVideo* av=(CSuiVideo*)FindChildByName(L"AVMainWnd");
			 MEDIA_INFO info=av->GetMeadiaInfo();
			 SSliderBar *SeekBar=(SSliderBar *)FindChildByName(L"slider_video_Seek");
			 
			 std::wstring CurTimeStr;
			
			 int h=(info.CurTime/(60*60));
			 int m=(info.CurTime%(60*60))/(60);
			 int s=((info.CurTime%(60*60))%(60));
			 wchar_t strtmp[100]=L"";
			 if(h<10){
				 swprintf_s(strtmp,L"0%d:",h);
				 CurTimeStr=strtmp;
			 }
			 else{
				 swprintf_s(strtmp,L"%d:",h);
				 CurTimeStr=strtmp;
			 }
			 if(m<10){
				 swprintf_s(strtmp,L"0%d:",m);
				 CurTimeStr+=strtmp;
			 }
			 else{				  
				 swprintf_s(strtmp,L"%d:",m);
				 CurTimeStr+=strtmp;
			 }

			 if(s<10){
				 swprintf_s(strtmp,L"0%d",s);
				 CurTimeStr+=strtmp;
			 }
			 else{
				 swprintf_s(strtmp,L"%d",s);
				 CurTimeStr+=strtmp;
			 }
			 CurTimeStr+=L"/";

			

			 
			  int Min=0;
			  int Max=0;
			  SeekBar->GetRange(&Min,&Max);
			  static std::wstring hh=L"00:00:00";
			  if(info.TotalTime>0)
			  {
                  SeekBar->SetRange(0,info.TotalTime);
				  h=info.TotalTime/(60*60);
				  m=(info.TotalTime%(60*60))/60;
				  s=((info.TotalTime%(60*60))%60);


				  if(h<10)
				  {
					  swprintf_s(strtmp,L"0%d:",h);
					  hh=strtmp;
				  }
				  else{
					  swprintf_s(strtmp,L"%d:",m);
					  hh=strtmp;
				  }
				  if(m<10){
					  swprintf_s(strtmp,L"0%d:",m);
					  hh+=strtmp;
				  }
				  else{
					  swprintf_s(strtmp,L"%d:",m);
					 hh+=strtmp;
				  }
				  if(s<10){
					  swprintf_s(strtmp,L"0%d:",s);
					 hh+=strtmp;
				  }
				  else{
					  swprintf_s(strtmp,L"%d",s);
					  hh+=strtmp;
				  }
			  }
			  if(info.CurTime>m_CumrTime&&info.serial==av->PktSerial())
			  {
			     if(m_CumrTime-m_lastSeekTime<=0&&info.CurTime-m_CumrTime<10)
				 {
                       SeekBar->SetValue(info.CurTime);
					    m_CumrTime=info.CurTime;
				 }else if(m_CumrTime-m_lastSeekTime>0){
                     SeekBar->SetValue(info.CurTime);
					  m_CumrTime=info.CurTime;
				 }
				
			  }else if(m_serial==info.serial)
			  {
                  
			  }
			  m_serial=info.serial;
			  m_SeekTimer++;
			  CurTimeStr+=hh;
			  SStatic* AV_CurTimeTxt=( SStatic  *)FindChildByName(L"AV_CurTimeTxt");
			  AV_CurTimeTxt->SetWindowText(CurTimeStr.c_str());

			  
		}
	}
	bool CMainDlg::OnSliderVideo(EventArgs *pEvt)
	{
		EventSliderPos *pEvt2 = sobj_cast<EventSliderPos>(pEvt);
		SASSERT(pEvt2);
		SSliderBar * pSlider = sobj_cast<SSliderBar>(pEvt->sender);
		SASSERT(pSlider);


		SSliderBar *VolBar=(SSliderBar *)FindChildByName(L"slider_video");
		if(m_PlayerState!=0)
		{
			m_SeekTimer=0;
			m_lastSeekTime=m_CumrTime;
			int vol=pEvt2->nPos-m_CumrTime;
			m_CumrTime+=vol;
			CSuiVideo* av=(CSuiVideo*)FindChildByName(L"AVMainWnd");
			av->AvSeek(pEvt2->nPos);
		}/**/
		return true;
	}
	bool CMainDlg::OnSliderAudio(EventArgs *pEvt)
	{
		EventSliderPos *pEvt2 = sobj_cast<EventSliderPos>(pEvt);
		SASSERT(pEvt2);
		SSliderBar * pSlider = sobj_cast<SSliderBar>(pEvt->sender);
		SASSERT(pSlider);

       
		SSliderBar *VolBar=(SSliderBar *)FindChildByName(L"AvAudio");
		if(m_PlayerState!=0)
		{
			long vol=pEvt2->nPos;
			CSuiVideo* av=(CSuiVideo*)FindChildByName(L"AVMainWnd");
			av->SetVolume(vol);
		}/**/
		return true;
	}
	int  CMainDlg::OpenMedia(char *path)
	{
		this->KillTimer(1);

		SStatic* AV_CurTimeTxt=( SStatic  *)FindChildByName(L"AV_CurTimeTxt");
		AV_CurTimeTxt->SetWindowText(L"00:00:00/00:00:00");
		SSliderBar *SeekBar=(SSliderBar *)FindChildByName(L"slider_video_Seek");
		CSuiVideo* av=(CSuiVideo*)FindChildByName(L"AVMainWnd");
		int ret=av->OpenMedia(path);
		if(ret==-1)
		{
			MessageBox(m_hWnd, L"文件打开错误", 0, MB_OK); 
			m_PlayerState=0;
			SImageButton* pPlay=(SImageButton*)FindChildByName(L"AVPlayBtn");
			pPlay->SetAttribute(L"skin",L"_skin.PLAY",TRUE);
			pPlay->UpdateWindow();
		}else
		{

			// SeekBar->SetRange(0,1);
			m_PlayerState=1;

			SImageButton* pPlay=(SImageButton*)FindChildByName(L"AVPlayBtn");
			pPlay->SetAttribute(L"skin",L"_skin.Pause",TRUE);
			pPlay->UpdateWindow();
			this->SetTimer(1,1000);
			SeekBar->SetValue(0);
			m_CumrTime=0;
		}
		return ret;
	}
    void CMainDlg::OnFolder()
	{
		m_CumrTime=0;
		SStatic* AV_CurTimeTxt=( SStatic  *)FindChildByName(L"AV_CurTimeTxt");
		AV_CurTimeTxt->SetWindowText(L"00:00:00/00:00:00");

		SSliderBar *SeekBar=(SSliderBar *)FindChildByName(L"slider_video_Seek");
		SeekBar->SetRange(0,10000);
		SeekBar->SetValue(0);
		wchar_t* filter = L"文件(*.mp4; *.avi; *.flv)\0*.mp4;*.avi; *.rmvb;*.flv\0全部 (*.*)\0*.*\0\0";  
		CFileDialog dlg(true, 0, 0, OFN_FILEMUSTEXIST|OFN_HIDEREADONLY|OFN_PATHMUSTEXIST, filter, m_hWnd);  
		if(dlg.DoModal() == IDOK)
		{  
			char path[1024]="";
			CChineseCode::wcharTochar(dlg.m_szFileName,path,1024);
			OpenMedia(path);
		}  
	}
	void CMainDlg::OnFileList()
	{
		MessageBox(m_hWnd, L"此功能暂时未实现", 0, MB_OK);  
	}

	void CMainDlg::OnDecelerate()
	{
         CSuiVideo* av=(CSuiVideo*)FindChildByName(L"AVMainWnd");
		 av->OnDecelerate();
	}
	void CMainDlg::OnAccelerate()
	{
		CSuiVideo* av=(CSuiVideo*)FindChildByName(L"AVMainWnd");
		av->OnAccelerate();
	}
	void CMainDlg::OnRButtonUp(UINT nFlags, CPoint point)
	{
		  SetMsgHandled(FALSE);
           CSuiVideo* av=(CSuiVideo*)FindChildByName(L"AVMainWnd");
		   CRect rt=av->GetClientRect();

		   SListView *pLstViewFix = FindChildByName2<SListView>("AV_list");
		   CRect rt2=pLstViewFix->GetClientRect();
		   if(rt.PtInRect(point)||rt2.PtInRect(point))
		   {
			   int i=0;
			   i++;
		   }
	}


	void CMainDlg::FullScreen()
	{
		if(!m_bFullScreen)
		{
              OnMaximize();
			  FindChildByName(L"AV_Tittle")->SetVisible(FALSE);
			  FindChildByName(L"SeekWin")->SetVisible(FALSE);
			  FindChildByName(L"ButtonToolbar")->SetVisible(FALSE);
			  FindChildByName(L"AV_list_pane")->SetVisible(FALSE);
			  FindChildByName(L"split_row")->SetAttribute(L"pos",L"0,0,-0,-0");
			  m_bFullScreen=true;
		}else
		{
			OnRestore();
			FindChildByName(L"AV_Tittle")->SetVisible(true);
			FindChildByName(L"SeekWin")->SetVisible(true);
			FindChildByName(L"ButtonToolbar")->SetVisible(true);
			FindChildByName(L"AV_list_pane")->SetVisible(true);
			FindChildByName(L"split_row")->SetAttribute(L"pos",L"0,[0,-0,]-0");
			m_bFullScreen=false;
		}
	}
	void CMainDlg::ScreenNormal()
	{
	   
		   OnRestore();
		   FindChildByName(L"AV_Tittle")->SetVisible(true);
		   FindChildByName(L"SeekWin")->SetVisible(true);
		   FindChildByName(L"ButtonToolbar")->SetVisible(true);
		   FindChildByName(L"AV_list_pane")->SetVisible(true);
		   FindChildByName(L"split_row")->SetAttribute(L"pos",L"0,[0,-0,]-0");
		   m_bScreenNormal=true;
	}
	void  CMainDlg::ScreenMini()
	{
		OnRestore();
		FindChildByName(L"AV_Tittle")->SetVisible(FALSE);
		FindChildByName(L"SeekWin")->SetVisible(FALSE);
		FindChildByName(L"ButtonToolbar")->SetVisible(FALSE);
		FindChildByName(L"AV_list_pane")->SetVisible(FALSE);
		FindChildByName(L"split_row")->SetAttribute(L"pos",L"0,0,-0,-0");
		m_bScreenNormal=false;
	}
	//DUI菜单响应函数
	void CMainDlg::OnCommand(UINT uNotifyCode, int nID, HWND wndCtl)
	{
		if(uNotifyCode==0)
		{
			if(nID == 1){
			   OnFolder();
			}else if(nID==2){
				
			}else if(nID==3){
				FullScreen();
			}else if(nID==4){
				ScreenNormal();
			}else if(nID==5){
				ScreenMini();
			}
		}
	}
}