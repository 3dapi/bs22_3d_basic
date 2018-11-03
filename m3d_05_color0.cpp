

#pragma comment(lib, "d3dx9.lib")

#include <stdio.H>
#include <stdlib.H>
#include <string.H>
#include <windows.H>

#include <d3dx9.H>


// Array Order
// R,G,B: FLOAT[0,1,2]
// C,M,Y: FLOAT[0,1,2]
// H,S,V: FLOAT[0,1,2]

void GrayFromRGB(FLOAT* pOut, const FLOAT* pIn)
{
	FLOAT gray = pIn[0] * 0.299f + pIn[1] * 0.587f + pIn[2] * 0.114f;
	
	pOut[0] = gray;
	pOut[1] = gray;
	pOut[2] = gray;
}

void RGBFromGray(FLOAT* pOut, const FLOAT* pIn)
{
	FLOAT gray = pIn[0];
	
	pOut[0] = gray;
	pOut[1] = gray;
	pOut[2] = gray;
}


void CMYKFromRGB(FLOAT* pOut, const FLOAT* pIn)
{
	FLOAT	C=0;
	FLOAT	M=0;
	FLOAT	Y=0;
	FLOAT	K=1;
	
	C = 1 - pIn[0];
	M = 1 - pIn[1];
	Y = 1 - pIn[2];
	
	if(C<K)	K=C;
	if(M<K)	K=M;
	if(Y<K) K=Y;
	
	if(1==K)
	{
		pOut[0] = 0;
		pOut[1] = 0;
		pOut[2] = 0;
		pOut[3] = K;
		return;
	}
	
	C = (C - K)/(1-K);
	M = (M - K)/(1-K);
	Y = (Y - K)/(1-K);
	
	
	pOut[0] = C;
	pOut[1] = M;
	pOut[2] = Y;
	pOut[3] = K;
}


void RGBFromCMYK(FLOAT* pOut, const FLOAT* pIn)
{
	FLOAT	R=0;
	FLOAT	G=0;
	FLOAT	B=0;
	
	FLOAT	C=pIn[0];
	FLOAT	M=pIn[1];
	FLOAT	Y=pIn[2];
	FLOAT	K=pIn[3];
	
	C = C * (1-K) + K;
	M = M * (1-K) + K;
	Y = Y * (1-K) + K;
	
	pOut[0] = 1 - C;
	pOut[1] = 1 - M;
	pOut[2] = 1 - Y;
}


// H: [0,360), S,V: [0,1]
void HSVFromRGB(FLOAT* pOut, const FLOAT* pIn)
{
	FLOAT	R = pIn[0];
	FLOAT	G = pIn[1];
	FLOAT	B = pIn[2];
	
	FLOAT	H = 0;
	FLOAT	S = 0;
	FLOAT	V = 0;
	
	
	FLOAT fMax = max( R, max(G, B));
	FLOAT fMin = min( R, min(G, B));
	FLOAT Del = fMax - fMin;	

	// Value: = max(R,G,B)
	V = fMax;


	if(0 == Del)
	{
		pOut[0] = 0;
		pOut[1] = 0;
		pOut[2] = V;
		return;
	}
	
	//Saturation
	S = (Del)/fMax;
	

	// H => (1/6)(0 + (B-R)/Del), or (1/6)(2 + (B-R)/Del) (4 + (B-R)/Del)


	// Between Yello, Magenta
	if(fMax == R)			H = 0 + (G - B) / Del;

	// Between Cyan, Yello
	else if(fMax == G)		H = 2 + (B - R) / Del;

	// Between Magenta Cyan
	else if(fMax == B)		H = 4 + (R - G) / Del;

	H *= 60;

	//H: [0,360)
	if(H ==360)		H = 0;
	else if(H <  0)	H += 360;
	else if(H >360)	H -= 360;
	
	pOut[0] = H;
	pOut[1] = S;
	pOut[2] = V;
}


void RGBFromHSV(FLOAT* pOut, const FLOAT* pIn)
{
	FLOAT	H = pIn[0];
	FLOAT	S = pIn[1];
	FLOAT	V = pIn[2];
	
	FLOAT	R = 0;
	FLOAT	G = 0;
	FLOAT	B = 0;
	
	
	// Black
	if(V == 0)
	{
		pOut[0] = 0;
		pOut[1] = 0;
		pOut[2] = 0;
		return;
	}

	// 회색
	if(S == 0)
	{
		pOut[0] = V;
		pOut[1] = V;
		pOut[2] = V;
		return;
	}


	FLOAT dH, t1, t2, t3;
	int nH;


	// H를 6등분한다.
	H /= 60;

	nH = (int)H;
	dH = H - nH;

	t1 = V * (1 - S);
	t2 = V * (1 - (S * dH));
	t3 = V * (1 - (S * (1 - dH)));
	
	if		(0 == nH){ R = V;	G = t3; B = t1;	}
	else if	(1 == nH){ R = t2;	G = V;	B = t1;	}
	else if	(2 == nH){ R = t1;	G = V;	B = t3;	}
	else if	(3 == nH){ R = t1;	G = t2; B = V;	}
	else if	(4 == nH){ R = t3;	G = t1; B = V;	}
	else			 { R = V;	G = t1;	B = t2;	}

	
	pOut[0] = R;
	pOut[1] = G;
	pOut[2] = B;
}

// H: [0,360), L,S: [0,1]
void HLSFromRGB(FLOAT* pOut, const FLOAT* pIn)
{
	FLOAT	R = pIn[0];
	FLOAT	G = pIn[1];
	FLOAT	B = pIn[2];
	
	FLOAT	H = 0;
	FLOAT	L = 0;
	FLOAT	S = 0;
	FLOAT	V = 0;
	
	
	FLOAT m;
	FLOAT vm;
	FLOAT r2, g2, b2;
	
	
	V = max(R, max(G, B));
	m = min(R, min(G, B));
	
	S = (m + V) / 2;
	
	if(S<= 0)
	{
		pOut[0] = H;
		pOut[1] = L;
		pOut[2] = S;
		return;
	}
	
	
	L = vm = V - m;
	
	if(L <= 0)
	{
		pOut[0] = H;
		pOut[1] = L;
		pOut[2] = S;
		return;
	}
	
	
	if(S <= 0.5)
		L /= (V + m );
	
	else
		L /= (2 - V - m) ;
	
	
	r2 = (V - R) / vm;
	g2 = (V - G) / vm;
	b2 = (V - B) / vm;
	
	if (R == V)
		H = (G == m ? 5 + b2 : 1 - g2);
	else if (G == V)
		H = (B == m ? 1 + r2 : 3 - b2);
	else
		H = (R == m ? 3 + g2 : 5 - r2);
	
	H *= 60;
	
	pOut[0] = H;
	pOut[1] = L;
	pOut[2] = S;
}


void RGBFromHLS(FLOAT* pOut, const FLOAT* pIn)
{
	FLOAT	H = pIn[0];
	FLOAT	L = pIn[1];
	FLOAT	S = pIn[2];
	
	FLOAT	R = 0;
	FLOAT	G = 0;
	FLOAT	B = 0;
	FLOAT	V = 0;
	
	
	if(S <= 0.5)
		V = (S * (1 + L));
	
	else
		V = (S + L - S * L);
	
	
	if (V <= 0) 
	{
		R = G = B = 0;
		pOut[0] = R;
		pOut[1] = G;
		pOut[2] = B;
		
		return;
	}
	
	
	FLOAT m;
	FLOAT sv;
	int sextant;
	FLOAT fract, vsf, mid1, mid2;
	
	m = S + S - V;
	sv = (V - m ) / V;
	H /= 60;
	sextant = (int)H;	
	fract = H - sextant;
	vsf = V * sv * fract;
	mid1 = m + vsf;
	mid2 = V - vsf;
	
	switch (sextant)
	{
	case 0: R = V;    G = mid1; B = m; break;
	case 1: R = mid2; G = V;    B = m; break;
	case 2: R = m;    G = V;    B = mid1; break;
	case 3: R = m;    G = mid2; B = V; break;
	case 4: R = mid1; G = m;    B = V; break;
	case 5: R = V;    G = m;    B = mid2; break;
	}
	
	pOut[0] = R;
	pOut[1] = G;
	pOut[2] = B;
}



void main()
{
	
	FLOAT Out[4]={0};
	
//	CMYKFromRGB( Out, (const FLOAT*)&D3DXCOLOR(0.9F, 0.58F, 0.24F, 1) );
//	printf( "CMY(%.3f, %.3f, %.3f, %.3f) <-- RGB(0.9F, 0.58F, 0.24F)\n", Out[0], Out[1], Out[2], Out[3]);
//	
//	RGBFromCMYK( Out, (const FLOAT*)&D3DXCOLOR(0.0F, 0.356F, 0.733F, 0.1F) );
//	printf( "CMY(%.3f, %.3f, %.3f) <-- RGB(0.0F, 0.356F, 0.733F, 0.1F)\n", Out[0], Out[1], Out[2]);
	
	HSVFromRGB( Out, (const FLOAT*)&D3DXCOLOR(.7F, .8F, .2F, 1) );
	printf( "HSV(%.3f, %.3f, %.3f) <-- RGB(.7F, .8F, .2F)\n", Out[0], Out[1], Out[2]);
	
	RGBFromHSV(Out, (const FLOAT*)&D3DXCOLOR(70.F, 0.75F, 0.8F, 1) );
	printf( "RGB(%.3f, %.3f, %.3f) <-- HLS(70.F, 0.75F, 0.8F)\n", Out[0], Out[1], Out[2]);
	
	HLSFromRGB( Out, (const FLOAT*)&D3DXCOLOR(0.2F, 0.4F, 0.9F, 1) );
	printf( "HLS(%.3f, %.3f, %.3f) <-- RGB(0.2F, 0.4F, 0.9F)\n", Out[0], Out[1], Out[2]);
	
	RGBFromHLS( Out, (const FLOAT*)&D3DXCOLOR(222.857f, 0.778F, 0.55F, 1) );
	printf( "RGB(%.3f, %.3f, %.3f) <-- HLS(222.857f, 0.778F, 0.55F)\n", Out[0], Out[1], Out[2]);
	
	
	printf( "\n----------------------------------------------------------\n\n");
	
	D3DXCOLOR Y(1.F, 1.F, 0.F, 1);
	D3DXCOLOR B(1.F, 0.F, 1.F, 1);
	D3DXCOLOR C;
	
	
	D3DXCOLOR Ycmrk;
	D3DXCOLOR Bcmrk;
	D3DXCOLOR Ccmrk;
	
	CMYKFromRGB( (FLOAT*)&Ycmrk, (const FLOAT*)&Y);
	CMYKFromRGB( (FLOAT*)&Bcmrk, (const FLOAT*)&B);
	
	
	Ccmrk.r = Ycmrk.r + Bcmrk.r;
	Ccmrk.g = Ycmrk.g + Bcmrk.g;
	Ccmrk.b = Ycmrk.b + Bcmrk.b;
	Ccmrk.a = Ycmrk.a + Bcmrk.a;
	
	if(Ccmrk.a>1)
		Ccmrk.a = 1;
	
	if(Ccmrk.r>1)	Ccmrk.r = 1;
	if(Ccmrk.g>1)	Ccmrk.g = 1;
	if(Ccmrk.b>1)	Ccmrk.b = 1;

	if(Ccmrk.r<0)	Ccmrk.r = 0;
	if(Ccmrk.g<0)	Ccmrk.g = 0;
	if(Ccmrk.b<0)	Ccmrk.b = 0;

	
	RGBFromCMYK( (FLOAT*)&C, (const FLOAT*)&Ccmrk);
	
	

	
	float	Yins[4];
	float	Bins[4];
	float	Brgh;
	GrayFromRGB( (FLOAT*)&Yins, (const FLOAT*)&Y);
	GrayFromRGB( (FLOAT*)&Bins, (const FLOAT*)&B);
	
	Brgh = Yins[0] * Bins[0];
	
	printf( "RGB(%.3f, %.3f, %.3f, %.3f)\n", C.r, C.g, C.b, Brgh);
}

