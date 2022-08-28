#include <cxxopts.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <shaderc/shaderc.hpp>

using namespace shaderc;
namespace fs = std::filesystem;

class shader_includer : public CompileOptions::IncluderInterface
{
public:
	shader_includer(fs::path _base_path)
		: base_path_(_base_path)
	{}

	virtual shaderc_include_result* GetInclude(
		const char* _requested_source,
		shaderc_include_type _type,
		const char* _requesting_source,
		size_t _include_depth) override
	{
		shaderc_include_result* include = new shaderc_include_result;

		auto include_file = base_path_ / fs::path{ _requested_source };

		{
			const std::string* source_name =
				scoped_strings_.emplace_back(new std::string("toto.vs"));

			include->source_name = source_name->data();
			include->source_name_length = source_name->size();
		}

		if(!fs::exists(include_file))
		{
			std::cout << "can't find " << include_file << std::endl;
			return include;
		}

		std::ifstream stream(include_file.c_str());
		if(!stream.is_open())
		{
			std::cout << "failed to open " << include_file << std::endl;
			return include;
		}
		else
		{
			const std::string* code = scoped_strings_.emplace_back(new std::string(
				std::istreambuf_iterator<char>(stream),
				std::istreambuf_iterator<char>{}));
			include->content = code->data();
			include->content_length = code->size();
		}

		return include;
	}

	virtual void ReleaseInclude(shaderc_include_result* include_result) override
	{
		delete include_result;
		for(auto str : scoped_strings_)
			delete str;
	}

private:
	fs::path base_path_;
	std::vector<std::string*> scoped_strings_;
};

int run_preprocessor(fs::path _input_path, fs::path _output_path)
{
	CompileOptions options;
	Compiler compiler;

	options.SetIncluder(std::make_unique<shader_includer>(_input_path.parent_path()));

	std::ifstream stream(_input_path.c_str());
	if(!stream.is_open())
	{
		std::cout << "failed to open " << _input_path << std::endl;
		return 0;
	}
	const std::string shader_code(std::istreambuf_iterator<char>(stream), {});

	const auto filepath = _input_path.string();
	PreprocessedSourceCompilationResult result = compiler.PreprocessGlsl(
		shader_code,
		shaderc_glsl_default_vertex_shader,
		filepath.c_str(),
		options);

	if(result.GetCompilationStatus() == shaderc_compilation_status_success)
	{
		std::ofstream stream(_output_path);
		if(!stream.is_open())
		{
			std::cout << "failed to open " << _input_path << std::endl;
			return 0;
		}

		std::copy(result.begin(), result.end(), std::ostream_iterator<char>(stream));

		std::string output(result.begin(), result.end());

		std::cout << output << std::endl;
		return 1;
	}
	else
	{
		std::cout << result.GetErrorMessage() << std::endl;
		return 0;
	}
}

int main(int argc, char** argv)
{
	std::string input_file;

	cxxopts::ParseResult parse_result;

	try
	{
		cxxopts::Options options_parser(
			"glsl-preprocessor-validator",
			"Tool to preprocess and validate OpenGL shader files");

		// clang-format off

		options_parser      
			.positional_help("input shader file")
			.show_positional_help()
			.add_options()
			("help", "Print help")
			("p", "Preprocess the shaders and output a file to the [output] path")
			("o,output", "Output file, only for preprocessor", cxxopts::value<std::string>()->default_value(""))
			("i,input", "Input file", cxxopts::value<std::string>(input_file))
			;

		// clang-format on

		options_parser.parse_positional({ "input" });

		parse_result = options_parser.parse(argc, argv);

		if(parse_result.count("help"))
		{
			std::cout << options_parser.help({ "" }) << std::endl;
			return 1;
		}
	}
	catch(const std::exception& e)
	{
		std::cout << "error parsing options: " << e.what() << std::endl;
		return 0;
	}

	if(input_file.empty())
	{
		std::cout << "no input file" << std::endl;
		return 0;
	}

	fs::path filepath(input_file);

	if(!fs::exists(filepath))
	{
		std::cout << "can't find " << filepath << std::endl;
		return 0;
	}
	else if(!fs::is_regular_file(filepath))
	{
		std::cout << "not a file " << filepath << std::endl;
		return 0;
	}

	if(parse_result.count("p"))
	{
		fs::path output = parse_result["output"].as<std::string>();
		if(fs::is_directory(output))
		{
			auto output_file_name = filepath.stem().string() + "_preprocess";
			if(filepath.has_extension())
				output_file_name += filepath.extension().string();

			output /= output_file_name;
		}

		return run_preprocessor(filepath, output);
	}

	return 1;
}