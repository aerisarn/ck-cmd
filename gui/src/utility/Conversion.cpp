#include <src/utility/Conversion.h>

#include <core/FBXWrangler.h>

using namespace ckcmd::HKX;
using namespace ckcmd::FBX;

AnimationInfo Conversion::convertFbxAnimationToHkx(
	const fs::path& skeleton_file,
	const fs::path& source_file, 
	const fs::path& output_directory
)
{
	AnimationInfo out;

	FBXWrangler wrangler;
	wrangler.setExternalSkeletonPath(skeleton_file.string());
	wrangler.ImportScene(source_file.string().c_str());

	fs::path out_path = output_directory / source_file.filename().replace_extension(".hkx");
	fs::create_directories(output_directory);
	auto result =  wrangler.SaveAnimation(out_path.string());
	if (!result.empty())
	{
		auto& outmovement = result.begin()->second;

		for (auto& traslation : outmovement.translations)
		{
			auto time = std::get<0>(traslation);
			auto value = std::get<1>(traslation);
			out.first.translations.insert({ time, {value(0), value(1), value(2)} });
		}

		for (auto& rotation : outmovement.rotations)
		{
			auto time = std::get<0>(rotation);
			auto value = std::get<1>(rotation);
			out.first.rotations.insert({ time, {value(0), value(1), value(2), value(3)} });
		}
		out.first.duration = outmovement.duration;

		out.second = out_path;
	}
	return out;
}


bool Conversion::convertHkxAnimationToFBX
(
	const fs::path& skeleton_file,
	const fs::path& source_file,
	const fs::path& output_directory,
	const AnimData::root_movement_t& movements
)
{
	FBXWrangler wrangler;
	wrangler.NewScene();
	FbxNode* skeleton_root = NULL;
	vector<FbxProperty> floats;
	vector<FbxNode*> ordered_skeleton = wrangler.importExternalSkeleton(skeleton_file.string(), "", floats);

	RootMovement movement;
	movement.duration = movements.duration;
	for (const auto& translation : movements.translations)
	{
		movement.translations.push_back({ translation.first, hkVector4(translation.second[0], translation.second[1], translation.second[2]) });
	}
	for (const auto& rotation : movements.rotations)
	{
		movement.rotations.push_back({ rotation.first, ::hkQuaternion(rotation.second[0], rotation.second[1], rotation.second[2], rotation.second[3])});
	}

	wrangler.importAnimationOnSkeleton(source_file.string(), ordered_skeleton, floats, movement);

	fs::path out_path = output_directory / source_file.filename().replace_extension(".fbx");

	return wrangler.ExportScene(out_path.string().c_str());
}

bool Conversion::convertHkxSkeletonToFbx(
	const fs::path& skeleton_file,
	const fs::path& output_directory
)
{
	if (fs::exists(skeleton_file) && fs::is_regular_file(skeleton_file))
	{
		FBXWrangler wrangler;
		wrangler.NewScene();
		vector<FbxProperty> floats;
		wrangler.setExportRig(true);
		vector<FbxNode*> ordered_skeleton = wrangler.importExternalSkeleton(skeleton_file.string(), "", floats);
		fs::path skeleton_nif = skeleton_file; skeleton_nif.replace_extension(".nif");
		//if (fs::exists(skeleton_nif) && fs::is_regular_file(skeleton_nif))
		//{
		//	NifFile mesh(skeleton_nif.string().c_str());
		//	wrangler.AddNif(mesh);
		//}
		fs::create_directories(output_directory);
		fs::path out_path = output_directory / fs::path(skeleton_file).filename().replace_extension(".fbx");
		wrangler.ExportScene(out_path.string().c_str());
		return true;
	}
	return false;
}