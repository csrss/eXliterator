
class ResImage // by Napalm
{
	private:
		// Private class variables
		HDC     m_hdcSurface;
		INT     m_ndcSurface;
		HBITMAP m_hbmSurface;
		BITMAP  m_bmSurface;
		
		// Load Images From Resources - Napalm @ NetCore2K
		// BMP, GIF, JPEG, and all the rest ;-)
		HBITMAP LoadImageFromResource(HINSTANCE hInst, LPTSTR lpszResName, LPTSTR lpszResType)
		{
			HRSRC   hResInfo   = FindResource(hInst, lpszResName, lpszResType);
			HGLOBAL hResGlobal = LoadResource(hInst, hResInfo);
			DWORD   dwSize     = SizeofResource(hInst, hResInfo);
			HGLOBAL hgImage    = GlobalAlloc(GMEM_MOVEABLE, dwSize);
			CopyMemory(GlobalLock(hgImage), LockResource(hResGlobal), dwSize);
			GlobalUnlock(hgImage); UnlockResource(hResGlobal);
			
			IStream *isImage = NULL;
			CreateStreamOnHGlobal(hgImage, TRUE, &isImage);
			if(isImage == NULL){ GlobalFree(hgImage); return NULL; }
			
			IPicture* pImage = NULL;
			OleLoadPicture(isImage, dwSize, FALSE, IID_IPicture, (LPVOID *)&pImage);
			isImage->Release();
			if(pImage == NULL) return NULL;
			
			HBITMAP hImage = NULL;
			pImage->get_Handle((OLE_HANDLE *)&hImage);
			
			HBITMAP hResult = (HBITMAP)CopyImage(hImage, IMAGE_BITMAP, 0, 0, 0);
			pImage->Release();
			return hResult;
		}
		
	public:
		// Constructor
		ResImage(HINSTANCE hInst, LPTSTR lpszResName, LPTSTR lpszResType)
		{
			m_hbmSurface = LoadImageFromResource(hInst, lpszResName, lpszResType);
			m_hdcSurface = CreateCompatibleDC(NULL);
			m_ndcSurface = SaveDC(m_hdcSurface);
			SelectBitmap(m_hdcSurface, m_hbmSurface);
			GetObject(m_hbmSurface, sizeof(BITMAP), &m_bmSurface);
		}

		// De-constructor
		~ResImage()
		{
			RestoreDC(m_hdcSurface, m_ndcSurface);
			DeleteDC(m_hdcSurface);
			DeleteBitmap(m_hbmSurface);
		}

		// Return the images' width
		INT Width()
		{
			return m_bmSurface.bmWidth;
		}

		// Return the images' height
		INT Height()
		{
			return m_bmSurface.bmHeight;
		}

		// Return the images' bits-per-pixel (bpp)
		INT BitsPixel()
		{
			return m_bmSurface.bmBitsPixel;
		}

		// Use these to access the image
		operator HDC()     { return m_hdcSurface; }
		operator HBITMAP() { return m_hbmSurface; }

};
