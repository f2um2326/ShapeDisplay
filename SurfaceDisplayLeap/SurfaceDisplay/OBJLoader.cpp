/**********************************************************************
　　OBJLoader.cpp

　　Alias Wavefornt OBJ File Loader

　　Date : Nov. 18, 2007
　　Version : 1.0
　　Author : Pocol
**********************************************************************/

//
//　include
//
#include <fstream>
#include "OBJLoader.h"
using namespace std;


//--------------------------------------------------------------------------------------------------
//　　GetDirectoryName
//　　Desc : 文字列からディレクトリを取得
//--------------------------------------------------------------------------------------------------
char * GetDirectoryName(const char *filename, char *dest)
{
	char *strings = NULL;
	char *dir;
	dir = new char [strlen(filename)+1];
	strcpy(dir, filename);
	if ( strings = strrchr(dir, '/' ) ) strings[1] = '\0';
	else if ( strings = strrchr(dir, '\\') ) strings[1] ='\0';
	else dir[0] = '\0';
	strcpy(dest, dir);
	return dest;
}

//---------------------------------------------------------------------------------------------------
//　　SetDirectory
//　　Desc : destの前にディレクトリを付加し，文字列を返す
//--------------------------------------------------------------------------------------------------
char * SetDirectoryName( char *dest, char *directory )
{
	char *s = "";
	s = new char [strlen(directory) + strlen(dest) + 1];
	strcpy( s, directory );
	strcat( s, dest );
	return s;
}

////////////////////////////////////////////////////////////////////////
//　　OBJMesh class
////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------------------
//　　OBJMesh
//　　Desc : コンストラクタ
//-------------------------------------------------------------------------------------------------
OBJMesh::OBJMesh()
{
	num_vertex = 0;
	num_normal = 0;
	num_face = 0;
	num_material = 0;
	listID = 0;
}

//-------------------------------------------------------------------------------------------------
//　　~OBJMesh
//　　Desc : デストラクタ
//-------------------------------------------------------------------------------------------------
OBJMesh::~OBJMesh()
{
}

//-------------------------------------------------------------------------------------------------
//　　Release
//　　Desc : メモリを解放
//-------------------------------------------------------------------------------------------------
void OBJMesh::Release()
{
	if ( vertex )
	{
		free(vertex);
		vertex = NULL;
	}
	if ( normal )
	{
		free(normal);
		normal = NULL;
	}
	if ( face )
	{
		free(face);
		face = NULL;
	}
	if ( material )
	{
		free(material);
		material = NULL;
	}
	if ( listID != 0 )
	{
		glDeleteLists(listID, 1);
		listID = 0;
	}
	num_vertex = 0;
	num_normal = 0;
	num_face = 0;
	num_material = 0;
}

//--------------------------------------------------------------------------------------------------
//　　AddVertex
//　　Desc : 頂点座標を追加
//--------------------------------------------------------------------------------------------------
void OBJMesh::AddVertex(OBJVertex &vert)
{
	num_vertex++;
	vertex = (OBJVertex*)realloc(vertex, num_vertex*sizeof(OBJVertex));
	vertex[num_vertex-1] = vert;
}

//-------------------------------------------------------------------------------------------------
//　　AddNormal
//　　Desc : 法線ベクトルを追加
//-------------------------------------------------------------------------------------------------
void OBJMesh::AddNormal(OBJVertex &norm)
{
	num_normal++;
	normal = (OBJVertex*)realloc(normal, num_normal*sizeof(OBJVertex));
	normal[num_normal-1] = norm;
}

//-------------------------------------------------------------------------------------------------
//　　AddFace
//　　Desc : 面を追加
//-------------------------------------------------------------------------------------------------
void OBJMesh::AddFace(OBJFace &surf)
{
	num_face++;
	face = (OBJFace*)realloc(face, num_face*sizeof(OBJFace));
	face[num_face-1] = surf;
}

//-------------------------------------------------------------------------------------------------
//　　AddMaterial
//　　Desc : マテリアルを追加
//-------------------------------------------------------------------------------------------------
void OBJMesh::AddMaterial(OBJMaterial &mat)
{
	num_material++;
	material = (OBJMaterial*)realloc(material, num_material*sizeof(OBJMaterial));
	material[num_material-1] = mat;
}

//-------------------------------------------------------------------------------------------------
//　　Information
//　　Desc : 情報を表示
//-------------------------------------------------------------------------------------------------
void OBJMesh::Information()
{
	cout << "* OBJファイル名：" << objFileName << endl;
	cout << "* MTLファイル名：" << mtlFileName << endl;
	cout << "* 頂点数：" << num_vertex << endl;
	cout << "* 法線数：" << num_normal << endl;
	cout << "* 面　数：" << num_face << endl;
	cout << "* 材質数：" << num_material << endl;
	cout << endl;
}

//-------------------------------------------------------------------------------------------------
//　　LoadOBJFile
//　　Desc : OBJファイルの読み込み
//-------------------------------------------------------------------------------------------------
bool OBJMesh::LoadOBJFile(const char *filename)
{
	ifstream file;
	int cmi = 0;	
	char tmp_char[OBJ_NAME_LENGTH];
	char buf[OBJ_BUFFER_LENGTH];
	char *pbuf;
	float min_size = 0.0;
	float max_size = 0.0;
	bool size_flag = false;
	
	//　オブジェクトファイル名をコピー
	strcpy(objFileName, filename);

	//　ファイルを開く
	file.open(filename, ios::in);
	if ( !file.is_open() )
	{
		cout << "Error : 指定されたOBJファイルが開けませんでした\n";
		cout << "File Name : " << filename << endl;
		return false;
	}

	//　ファイルの末端までループ
	while ( !file.eof() )
	{
		OBJVertex tmp_vert(0.0, 0.0, 0.0);
		OBJVertex tmp_norm(0.0, 0.0, 0.0);
		OBJFace tmp_face;
		float tmp_float=0.0;

		//　1行読み取り
		file.getline(buf, sizeof(buf));

		//　バッファの1文字目で判別
		switch ( buf[0] )
		{
		case 'v':
			//　バッファの2文字目で判別
			switch ( buf[1] )
			{
			//　Vertex
			case ' ':
				//　頂点座標を読み取り
				if ( sscanf(buf+2, "%f %f %f %f", &tmp_vert.x, &tmp_vert.y, &tmp_vert.z, &tmp_float) != 4 )
				{
					if ( sscanf(buf+2, "%f %f %f", &tmp_vert.x, &tmp_vert.y, &tmp_vert.z) != 3 )
					{
						cout << "Error : 頂点座標の数が不正です\n";
						return false;
					}
				}
				//　初期値の設定
				if ( !size_flag )
				{
					min_size = tmp_vert.x;
					max_size = tmp_vert.x;
					size_flag = true;
				}
				//　最大・最小の比較
				for ( int i=0; i<3; i++ )
				{
					if ( min_size > tmp_vert.v[i] ) min_size = tmp_vert.v[i];
					if ( max_size < tmp_vert.v[i] ) max_size = tmp_vert.v[i];
				}
				//　頂点座標を追加
				AddVertex(tmp_vert);
				break;

			//　Normal
			case 'n':
				//　法線ベクトルの読み取り
				if ( sscanf(buf+2, "%f %f %f", &tmp_norm.x, &tmp_norm.y, &tmp_norm.z) != 3)
				{
					cout << "Error : 法線ベクトルの数が不正です\n";
					return false;
				}
				//　法線ベクトルを追加
				AddNormal(tmp_norm);
				break;
			}
			break;

		//　face
		case 'f':
			pbuf = buf;
			//　空白の数で要素数がいくつあるかカウント
			while ( *pbuf )
			{
				if ( *pbuf == ' ' ) tmp_face.element++;
				pbuf++;
			}
			//　要素数3未満なら面を構成できない
			if ( tmp_face.element < 3 )
			{
				cout << "Error : 面を構成するための要素数が不正です\n";
				return false;
			}
			switch ( tmp_face.element )
			{
			//　三角形
			case 3:
				tmp_face.type = GL_TRIANGLES;
				break;

			//　四角形
			case 4:
				tmp_face.type = GL_QUADS;
				break;

			//　多角形
			default:
				tmp_face.type = GL_POLYGON;
				break;
			}
			//　インデックス用のメモリを確保
			tmp_face.vertex_index = new int [tmp_face.element];
			tmp_face.normal_index = new int [tmp_face.element];
			pbuf = buf;
			for ( int i=0; i<tmp_face.element; i++ )
			{
				pbuf = strchr(pbuf, ' ');
				pbuf++;

				//　構成要素の読み取り
				if ( sscanf(pbuf, "%d/%d/%d", &tmp_face.vertex_index[i], &tmp_float, &tmp_face.normal_index[i] ) != 3 )
				{
					if ( sscanf(pbuf, "%d//%d", &tmp_face.vertex_index[i], &tmp_face.normal_index[i] ) != 2 )
					{
						if ( sscanf(pbuf, "%d/%d", &tmp_face.vertex_index[i], &tmp_float ) != 2 )
						{
							sscanf(pbuf, "%d", &tmp_face.vertex_index[i]);
							tmp_face.use_normal = false;
						}
						else
						{
							tmp_face.use_normal = false;
						}
					}
					else
					{
						tmp_face.use_normal = true;
					}
				}
				else
				{
					tmp_face.use_normal = true;
				}
				//　配列の番号と合わせる
				tmp_face.vertex_index[i]--;
				if ( tmp_face.use_normal ) tmp_face.normal_index[i]--;
			}
			//　マテリアルインデックスを格納
			tmp_face.material_index = cmi;
			//　面を追加
			AddFace(tmp_face);
			break;

		//　usemtl
		case 'u':
			//　マテリアル名を読み取り
			strcpy(tmp_char, " ");
			sscanf(buf, "usemtl %s", &tmp_char);
			//　マテリアル名から検索
			for ( int i=0; i<num_material; i++ )
			{
				//　名前が一致したらマテリアル番号を格納
				if ( strcmpi(material[i].name, tmp_char) == 0 ) cmi = i;
			}
			break;

		//　mtllib
		case 'm':
			//　マテリアルファイル名を読み取り
			strcpy(tmp_char, " ");
			sscanf(buf, "mtllib %s", &tmp_char);
			//　マテリアルファイルの読み込み
			if ( !LoadMTLFile(
				SetDirectoryName(tmp_char, directoryName)	//　ディレクトリを付加
				))
				return false;
			break;
		
		default:
			break;
		}
	}

	//　サイズ調整用変数
	size = max_size - min_size;
	
	//　ファイルを閉じる
	file.close();

	return true;
}

//-------------------------------------------------------------------------------------------------
//　　LoadMTLFile
//　　Desc : MTLファイルの読み込み
//-------------------------------------------------------------------------------------------------
bool OBJMesh::LoadMTLFile(const char *filename)
{
	ifstream file;
	int cmi = -1;
	char buf[OBJ_BUFFER_LENGTH];
	float tmp_float=0.0f;
	OBJMaterial tmp_mat;

	//　マテリアルファイル名をコピー
	strcpy(mtlFileName, filename); 

	//　ファイルを開く
	file.open(filename, ios::in);
	if ( !file.is_open() )
	{
		cout << "Error : 指定されたMTLファイルが開けませんでした\n";
		cout << "File Name : " << filename << endl;
		return false;
	}

	//　ファイルの末端までループ
	while ( !file.eof() )
	{
		//　1行読み取り
		file.getline(buf, sizeof(buf));

		//　バッファの1文字目で判断
		switch ( buf[0] )
		{
		//　newmtl
		case 'n':
			cmi++;
			if ( cmi != 0 ) AddMaterial(tmp_mat);
			sscanf(buf, "newmtl %s", tmp_mat.name);
			break;

		//　Ka, Kd, Ks
		case 'K':
			switch ( buf[1] )
			{
			//　Ambient
			case 'a':
				sscanf(buf, "Ka %f %f %f", &tmp_mat.ambient.r, &tmp_mat.ambient.g, &tmp_mat.ambient.b);
				tmp_mat.color = tmp_mat.diffuse;
				break;

			//　Diffuse
			case 'd':
				sscanf(buf, "Kd %f %f %f", &tmp_mat.diffuse.r, &tmp_mat.diffuse.g, &tmp_mat.diffuse.b);
				break;

			//　Specular
			case 's':
				sscanf(buf, "Ks %f %f %f", &tmp_mat.specular.r, &tmp_mat.specular.g, &tmp_mat.specular.b);
				break;
			}
			break;

		//　d
		case 'd':
			if ( sscanf(buf, "d %f", &tmp_float) == 1 )
			{
				tmp_mat.color.a = tmp_float;
				tmp_mat.ambient.a = tmp_float;
				tmp_mat.diffuse.a = tmp_float;
				tmp_mat.specular.a = tmp_float;
				tmp_mat.emission.a = tmp_float;
			}
			break;

		//　Tr
		case 'T':
			if ( buf[1] == 'r' )
			{
				 sscanf(buf, "Tr %f %f %f", &tmp_mat.emission.r, tmp_mat.emission.g, tmp_mat.emission.b);
			}
			break;

		//　Ni
		case 'N':
			if ( buf[1] == 'i' )
			{
				sscanf(buf, "Ni %f", &tmp_mat.shininess);
			}
			break;

		//　該当なし
		default:
			break;
		}
	}

	//　最後に読みとったマテリアルを追加
	AddMaterial(tmp_mat);
	
	//　ファイルを閉じる
	file.close();

	return true;
}

//-------------------------------------------------------------------------------------------------
//　　Load
//　　Desc : ファイルのロード
//-------------------------------------------------------------------------------------------------
bool OBJMesh::Load(const char *objfilename)
{
	//　
	Release();

	//　一度メモリを確保
	vertex = (OBJVertex*)malloc(1*sizeof(OBJVertex));
	normal = (OBJVertex*)malloc(1*sizeof(OBJVertex));
	face = (OBJFace*)malloc(1*sizeof(OBJFace));
	material = (OBJMaterial*)malloc(1*sizeof(OBJMaterial));

	//　ディレクトリを切り抜き
	GetDirectoryName(objfilename, directoryName);
	
	//　OBJファイルのロード
	if ( !LoadOBJFile(objfilename) ) return false;

	return true;
}

//-------------------------------------------------------------------------------------------------
//　　Render
//　　Desc : 描画処理
//-------------------------------------------------------------------------------------------------
void OBJMesh::Render(float scale)
{
	int pre_mat = -1, cur_mat = 0;
	
	for ( int i=0; i<num_face; i++ )
	{
		//　マテリアルがあるとき
		if ( num_material > 0 )
		{
			//　インデックスを格納
			cur_mat = face[i].material_index;

			//　前と異なる色のとき
			if ( pre_mat != cur_mat )
			{
				//　Object Color
				glColor4fv(material[cur_mat].color);

				//　Ambient Color
				glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, material[cur_mat].ambient);
				
				//　Diffuse Color
				glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, material[cur_mat].diffuse);
				
				//　Specular Color
				glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, material[cur_mat].specular);

				//　Emission
				glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, material[cur_mat].emission);
				
				//　Shininess
				glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, material[cur_mat].shininess);
				
				//　更新
				pre_mat = cur_mat;
			}
		}

		//　面の描画を開始
		glBegin(face[i].type);
		for ( int j=0; j<face[i].element; j++ )
		{
			//　法線ベクトル
			if ( face[i].use_normal ) glNormal3fv(normal[face[i].normal_index[j]]);
			
			//　頂点
			glVertex3fv(	vertex[face[i].vertex_index[j]]*scale/size);
		}
		//　面の描画を終了
		glEnd();
	}

}

//--------------------------------------------------------------------------------------------------
//　　MakeDisplayList
//　　Desc : ディスプレイリストを作成
//--------------------------------------------------------------------------------------------------
GLuint OBJMesh::MakeDisplayList(float scale)
{
	//　リストIDを生成
	listID = glGenLists(1);

	//　ディスプレイリストの作成開始
	glNewList(listID, GL_COMPILE);

	//　描画
	Render(scale);

	//　ディスプレイリストの作成終了
	glEndList();

	//　リストIDを返す
	return listID;
}