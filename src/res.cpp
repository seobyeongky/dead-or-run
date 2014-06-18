#include "res.h"
#include "util.h"

#include <ole2.h>
#include <xmllite.h>
#include <stdio.h>
#include <shlwapi.h>

struct xmlElement_t
{
	wstring							name;
	list<pair<wstring, wstring>>	attr;
};

enum xmlMsgType_t
{
	XML_MSG_ELEMENT,
	XML_MSG_ELEMENT_END
};

struct xmlMsg_t
{
	xmlMsgType_t	type;
	xmlElement_t	element;
};

class XmlFile
{
public:
	XmlFile(LPCWSTR image_data_file)
		: file_stream(nullptr),
		reader(nullptr)
	{
		HRESULT hr;
		//Open read-only input stream
		if (FAILED(hr = SHCreateStreamOnFile(image_data_file, STGM_READ, &file_stream)))
		{
			ErrorMsg(L"Error creating file reader, error is %08.8lx", hr);
			return;
		}

		if (FAILED(hr = CreateXmlReader(__uuidof(IXmlReader), (void**) &reader, NULL)))
		{
			ErrorMsg(L"Error creating xml reader, error is %08.8lx", hr);
			return;
		}

		if (FAILED(hr = reader->SetProperty(XmlReaderProperty_DtdProcessing, DtdProcessing_Prohibit)))
		{
			ErrorMsg(L"Error setting XmlReaderProperty_DtdProcessing, error is %08.8lx", hr);
			return;
		}

		if (FAILED(hr = reader->SetInput(file_stream)))
		{
			ErrorMsg(L"Error setting input for reader, error is %08.8lx", hr);
			return;
		}
	}
	~XmlFile()
	{
		if(file_stream != nullptr) file_stream->Release();
		if(reader != nullptr) reader->Release();
	}

	bool Parse(xmlMsg_t *msg)
	{
		HRESULT			hr;
		XmlNodeType		node_type;
		LPCWSTR			pwszLocalName = nullptr;
		LPCWSTR			pwszValue = nullptr;

RETRY:
		msg->element.attr.clear();

		hr = reader->Read(&node_type);
		if(hr != S_OK)
			return false;

		switch (node_type)
        {
        case XmlNodeType_Element:
			msg->type = XML_MSG_ELEMENT;
            if (FAILED(hr = reader->GetLocalName(&pwszLocalName, NULL)))
            {
                ErrorMsg(L"Error getting local name, error is %08.8lx", hr);
				return false;
            }
			msg->element.name = pwszLocalName;
			std::transform(msg->element.name.begin(), msg->element.name.end(),msg->element.name.begin(), ::tolower);

			hr = reader->MoveToFirstAttribute();
			if(hr == S_FALSE)
				break;
			if(hr != S_OK)
			{
				ErrorMsg(L"Error moving to first attribute, error is %08.8lx", hr);
				return false;
			}
			else
			{
				while(TRUE)
				{
					if(!reader->IsDefault())
					{
		                if (FAILED(hr = reader->GetLocalName(&pwszLocalName, NULL)))
						{
							ErrorMsg(L"Error getting local name, error is %08.8lx", hr);
							return false;
						}
		                if (FAILED(hr = reader->GetValue(&pwszValue, NULL)))
		                {
							ErrorMsg(L"Error getting value, error is %08.8lx", hr);
							return false;
						}
						pair<wstring, wstring> pair(pwszLocalName, pwszValue);
						std::transform(pair.first.begin(), pair.first.end(), pair.first.begin(), ::tolower);
						std::transform(pair.second.begin(), pair.second.end(), pair.second.begin(), ::tolower);
						msg->element.attr.push_back(pair);
					}
					hr = reader->MoveToNextAttribute();
					if(hr == S_FALSE)
						break;
					else if(hr != S_OK)
					{
						ErrorMsg(L"Error moving to next attribute, error is %08.8lx", hr);
						return false;
					}
				}
			}
            break;

        case XmlNodeType_EndElement:
			msg->type = XML_MSG_ELEMENT_END;
            if (FAILED(hr = reader->GetLocalName(&pwszLocalName, NULL)))
            {
                ErrorMsg(L"Error getting local name, error is %08.8lx", hr);
				return false;
            }

			msg->element.name = pwszLocalName;
			std::transform(msg->element.name.begin(), msg->element.name.end(),msg->element.name.begin(), ::tolower);
			break;

		default:
			goto RETRY;
        }

		return true;
	}

private:
	IStream		*file_stream;
	IXmlReader	*reader;
};

SMap<wstring, Texture *>		texture_map;
map<wstring, Sprite *>			sprite_map;


bool LoadImagesFrom(LPCWSTR image_data_file)
{
	Texture		*texture = nullptr;
	xmlMsg_t	msg;
	bool		imageset_inside = false;
	XmlFile		file(image_data_file);

	while(file.Parse(&msg))
	{
		switch(msg.type)
		{
		case XML_MSG_ELEMENT:
			if(imageset_inside)
			{
				if(msg.element.name != L"image")
				{
					ErrorMsg(L"Imageset안에 image가 아닌 것이 있습니다. [%s]", msg.element.name.c_str());
					return false;
				}
				wstring name;
				
				int width, height, x_pos, y_pos;
				int set = 0;

				for(auto i = msg.element.attr.begin(); i != msg.element.attr.end(); ++i)
				{
					if(i->first == L"name")
					{
						name = i->second;
						assert((set & 0x001) == 0);
						set += 0x001;
					}
					else if(i->first == L"width")
					{
						width =  _wtoi(i->second.c_str());
						assert((set & 0x002) == 0);
						set += 0x002;
					}
					else if(i->first == L"height")
					{
						height = _wtoi(i->second.c_str());
						assert((set & 0x004) == 0);
						set += 0x004;
					}
					else if(i->first == L"xpos")
					{
						x_pos = _wtoi(i->second.c_str());
						assert((set & 0x008) == 0);
						set += 0x008;
					}
					else if(i->first == L"ypos")
					{
						y_pos = _wtoi(i->second.c_str());
						assert((set & 0x010) == 0);
						set += 0x010;
					}
				}

				if((set & 0x01f) == 0x01f)
				{
					sprite_map[name] = new Sprite(*texture, IntRect(x_pos, y_pos, width, height));
				}
				else
				{
					ErrorMsg(L"텍스처 정보 파일 오염");
					return false;
				}
			}
			else
			{
				if(msg.element.name != L"imageset")
				{
					ErrorMsg(L"Imageset이 아닌 것이 있습니다. [%s]", msg.element.name.c_str());
					return false;
				}
				for(auto i = msg.element.attr.begin(); i != msg.element.attr.end(); ++i)
				{
					if(i->first == L"imagefile")
					{
						SMap<wstring, Texture *>::Iter iter;
						const wstring & texture_file_name = i->second;

						if(!texture_map.Find(texture_file_name))
						{
							//기존 텍스쳐 리스트에 없는 파일인 경우입니다.
							texture = new Texture();
							string ansi_file_name(texture_file_name.begin(), texture_file_name.end());
							if(!texture->loadFromFile(ansi_file_name))
							{
								ErrorMsg(L"%s 텍스쳐 파일 로드 실패!", texture_file_name.c_str());
								return false;
							} 
							texture_map.Insert(texture_file_name, texture);
						}
						break;
					}
				}
				if(texture == nullptr)
				{
					ErrorMsg(L"Imageset에서 텍스쳐 파일 정보를 찾지 못했습니다!");
					return false;
				}
				imageset_inside = true;
			}
			break;

		case XML_MSG_ELEMENT_END:
			imageset_inside = false;
			break;
		}
	}
	return true;
}

bool LoadImages(LPCWSTR image_dir, LPCWSTR image_set_file)
{
	DirChanger		dir_changer(image_dir);
	xmlMsg_t		msg;
	bool			fileset_inside = false;
	XmlFile			file(image_set_file);

	while(file.Parse(&msg))
	{
		switch(msg.type)
		{
		case XML_MSG_ELEMENT:
			if( ! fileset_inside)
			{
				if(msg.element.name == "fileset")
					fileset_inside = true;
				else
				{
					ErrorMsg(L"Fileset이 아니라고?");
					return false;
				}
			}
			else
			{
				if(msg.element.name != "xmlfile")
				{
					ErrorMsg(L"xmlfile이 아니라고?");
					return false;
				}
				else
				{
					for(auto i = msg.element.attr.begin(); i != msg.element.attr.end(); ++i)
					{
						if(i->first == L"name")
						{
							if( ! LoadImagesFrom(i->second.c_str()))
							{
								ErrorMsg(L"%s로부터 이미지 로드 실패", i->second.c_str());
							}
						}
					}
				}
			}
			break;
		case XML_MSG_ELEMENT_END:
			fileset_inside = false;
			break;
		}
	}

	return true;
}

bool LoadResources()
{
	if (!normal_font.loadFromFile("font\\NanumGothic.ttf"))
	{
		ErrorMsg(L"font load 실패!");
		return false;
	}
	
	if(!LoadImages(L"image\\", L"file_info.xml"))
	{
		ErrorMsg(L"image load 실패!");
		return false;
	}

	if(!(audio_device = audiere::OpenDevice()))
	{
		ErrorMsg(L"audio device 열기 실패!");
	}

	if(!(bg_music = audiere::OpenSound(audio_device, "sound\\bgm.ogg", true)))
	{
		ErrorMsg(L"배경음악 열기 실패!");
	}


	return true;
}

void ReleaseResource()
{
	for(auto it = sprite_map.begin(); it != sprite_map.end(); ++it)
		delete it->second;
	sprite_map.clear();

	for(auto iter = texture_map.Begin(); iter != texture_map.End(); ++iter)
		delete (*iter).GetEl();
	texture_map.Clear();
}