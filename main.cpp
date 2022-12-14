#include "DOA6/G1mFile.h"
#include "debug.h"

std::string program_dir;
bool export_vgmap = false;

static bool text_oid_exists(const std::string &path)
{
    if (!Utils::FileExists(path))
        return false;

    size_t size;
    uint8_t *buf;

    buf = Utils::ReadFile(path, &size, false);
    if (!buf)
        return false;

    bool ret = true;

    for (size_t i = 0; i < size; i++)
    {
        if (buf[i] == 0)
        {
            ret = false;
            break;
        }
    }

    delete[] buf;
    return ret;
}

static void load_bone_names(const std::string &path, G1mFile &g1m)
{
    std::string oid_file = path.substr(0, path.length()-3) + "oid";
    std::string costume_oid;
    std::string hair_oid;
    std::string face_oid;

    int type = -1; // 0 = costume, 1 = hair, 2 = face

    if (Utils::FileExists("costume.oid"))
    {
        costume_oid = "costume.oid";
    }
    else
    {
        std::string path = Utils::MakePathString(program_dir, "costume.oid");

        if (Utils::FileExists(path))
        {
            costume_oid = path;
        }
    }

    if (Utils::FileExists("hair.oid"))
    {
        hair_oid = "hair.oid";
    }
    else
    {
        std::string path = Utils::MakePathString(program_dir, "hair.oid");

        if (Utils::FileExists(path))
        {
            hair_oid = path;
        }
    }

    if (Utils::FileExists("face.oid"))
    {
        face_oid = "face.oid";
    }
    else
    {
        std::string path = Utils::MakePathString(program_dir, "face.oid");

        if (Utils::FileExists(path))
        {
            face_oid = path;
        }
    }

    std::string fn = Utils::GetFileNameString(path);

    if (fn.find("_HAIR_") != std::string::npos)
    {
        type = 1;
    }
    else if (fn.find("_FACE_") != std::string::npos)
    {
        type = 2;
    }
    else if ((g1m.GetNumBonesID() >= 800 && g1m.GetNumBonesID() <= 900) ||
             (fn.find("_COS_") != std::string::npos && fn.find("OUTGAME") == std::string::npos && fn.find("CHRSEL") == std::string::npos))
    {
        type = 0;
    }

    if (!text_oid_exists(oid_file) || !g1m.LoadBoneNames(oid_file))
    {
        g1m.SetDefaultBoneNames();

        if (type == 0)
        {
            if (costume_oid.length() == 0 || !g1m.LoadBoneNames(costume_oid))
            {
                g1m.SetDefaultBoneNames();
            }
        }

        else if (type == 1)
        {
            if (hair_oid.length() == 0 || !g1m.LoadBoneNames(hair_oid))
            {
                g1m.SetDefaultBoneNames();
            }
        }

        else if (type == 2)
        {
            if (face_oid.length() == 0 || !g1m.LoadBoneNames(face_oid))
            {
                g1m.SetDefaultBoneNames();
            }
        }
    }
}

static bool massive_g1m_visitor(const std::string &path, bool, void *)
{
    if (Utils::EndsWith(path, ".g1m", false))
    {
        G1mFile g1m;

        DPRINTF("Exporting %s\n", Utils::GetFileNameString(path).c_str());

        if (!g1m.LoadFromFile(path))
            return false;
		
		load_bone_names(path, g1m);

        std::string dir_path = path.substr(0, path.length()-4);
        Utils::Mkdir(dir_path);

        if (!g1m.ExportTo3DM(dir_path, export_vgmap))
            return false;
    }

    return true;
}

bool export_g1m_data(const std::string &path)
{
	if (Utils::DirExists(path))
	{
		bool ret = Utils::VisitDirectory(path, true, false, false, massive_g1m_visitor);
		if (ret)
			UPRINTF("Success!\n");
		else
			UPRINTF("Some error happened\n");
		
		return ret;
	}
	
	if (!Utils::EndsWith(path, ".g1m", false))
	{
		DPRINTF("Error: File should have .g1m extension.\n");
		return false;
	}
	
	G1mFile g1m;	
	std::string dir_path = path.substr(0, path.length()-4);
	
	Utils::Mkdir(dir_path);
	
	g1m.SetParseNun(false);
	
	if (!g1m.LoadFromFile(path))
		return false;
	
	load_bone_names(path, g1m);
	
	if (!g1m.ExportTo3DM(dir_path, export_vgmap))
		return false;
	
	UPRINTF("Success!\n");
	return true;
}

int main(int argc, char *argv[])
{
    bool bad_usage = false;
	std::string file;
	
	if (argc == 2)
	{
		file = argv[1];
		if (file == "-vg")
			bad_usage = true;
	}
	else if (argc == 3)
	{
		if (strcmp(argv[1], "-vg") != 0)
		{
			DPRINTF("Unrecognized option \"%s\"\n", argv[1]);
			bad_usage = true;
		}
		else
		{
			export_vgmap = true;
		}
		
		file = argv[2];
	}
	else
	{
		bad_usage = true;
	}
	
	if (bad_usage)
    {
        DPRINTF("Bad usage. Usage: [-vg] %s file|dir\n", argv[0]);
		UPRINTF("Press enter to exit.");
		getchar();
        return -1;
    }
	
	 if (!strchr(argv[0], '\\') && !strchr(argv[0], '/'))
    {
        program_dir = "./";
    }
    else
    {
        program_dir = Utils::GetDirNameString(argv[0]);
    }

    int ret = export_g1m_data(Utils::NormalizePath(file));

	fseek(stdin, 0, SEEK_END);
	UPRINTF("Press enter to exit.");
    getchar();

    return ret;
}
