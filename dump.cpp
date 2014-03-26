/*
    This file is part of Spike Guard.

    Spike Guard is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Spike Guard is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Spike Guard.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "pe.h"

namespace sg 
{

// ----------------------------------------------------------------------------

/**
 *     @brief  Convenience function used to translate an integer to a list of flag and display
 *             it nicely.
 *
 *     @param  std::ostream& sink The output stream to write to.
 *     @param  boost::uint32_t value The value to translate.
 *     @param  const nt::flag_dict& dict The dictionary to use in order to translate the value.
 *     @param  const std::string& padding The padding to prepend to every string after the first one.
 *			   This is used to display flags as a column. 
 */
void pretty_print_flags(std::ostream& sink, boost::uint32_t value, const nt::flag_dict& dict, const std::string& padding = "\t\t\t")
{
	std::vector<std::string> flags = nt::translate_to_flags(value, dict);
	if (flags.size() > 0) 
	{
		for (std::vector<std::string>::iterator it = flags.begin() ; it != flags.end() ; ++it) 
		{
			if (it != flags.begin()) {
				sink << padding;
			}
			sink << *it << std::endl;
		}
	}
	else {
		sink << value << std::endl;
	}
}

// ----------------------------------------------------------------------------

void PE::dump_dos_header(std::ostream& sink) const
{
	if (!_initialized) {
		return;
	}

	sink << "DOS HEADER:" << std::endl << "-----------" << std::endl;
	sink << std::hex;
	sink << "e_magic\t\t" << _h_dos.e_magic[0] << _h_dos.e_magic[1] << std::endl;
	sink << "e_cblp\t\t" << _h_dos.e_cblp << std::endl;
	sink << "e_cp\t\t" << _h_dos.e_cp << std::endl;
	sink << "e_crlc\t\t" << _h_dos.e_crlc << std::endl;
	sink << "e_cparhdr\t" << _h_dos.e_cparhdr << std::endl;
	sink << "e_minalloc\t" << _h_dos.e_minalloc << std::endl;
	sink << "e_maxalloc\t" << _h_dos.e_maxalloc << std::endl;
	sink << "e_ss\t\t" << _h_dos.e_ss << std::endl;
	sink << "e_sp\t\t" << _h_dos.e_sp << std::endl;
	sink << "e_csum\t\t" << _h_dos.e_csum << std::endl;
	sink << "e_ip\t\t" << _h_dos.e_ip << std::endl;
	sink << "e_cs\t\t" << _h_dos.e_cs << std::endl;
	sink << "e_lfarlc\t" << _h_dos.e_lfarlc << std::endl;
	sink << "e_ovno\t\t" << _h_dos.e_ovno << std::endl;
	sink << "e_oemid\t\t" << _h_dos.e_oemid << std::endl;
	sink << "e_oeminfo\t" << _h_dos.e_oeminfo << std::endl;
	sink << "e_lfanew\t" << _h_dos.e_lfanew << std::endl <<std::endl;
}

// ----------------------------------------------------------------------------

void PE::dump_pe_header(std::ostream& sink) const
{
	if (!_initialized) {
		return;
	}

	std::vector<std::string> flags;

	sink << "PE HEADER:" << std::endl << "----------" << std::endl;
	sink << std::hex;
	sink << "Signature\t\t" << _h_pe.Signature << std::endl;
	sink << "Machine\t\t\t" << nt::translate_to_flag(_h_pe.Machine, nt::MACHINE_TYPES) << std::endl;
	sink << "NumberofSections\t" << _h_pe.NumberofSections << std::endl;
	sink << "TimeDateStamp\t\t" << _h_pe.TimeDateStamp << std::endl;
	sink << "PointerToSymbolTable\t" << _h_pe.PointerToSymbolTable << std::endl;
	sink << "NumberOfSymbols\t\t" << _h_pe.NumberOfSymbols << std::endl;
	sink << "SizeOfOptionalHeader\t" << _h_pe.SizeOfOptionalHeader << std::endl;

	sink << "Characteristics\t\t";
	pretty_print_flags(sink, _h_pe.Characteristics, nt::PE_CHARACTERISTICS);
	sink << std::endl;
}

// ----------------------------------------------------------------------------

void PE::dump_image_optional_header(std::ostream& sink) const
{
	if (!_initialized) {
		return;
	}

	std::vector<std::string> flags;

	sink << "IMAGE OPTIONAL HEADER:" << std::endl << "----------------------" << std::endl;
	sink << std::hex;
	sink << "Magic\t\t\t\t" << nt::translate_to_flag(_ioh.Magic, nt::IMAGE_OPTIONAL_HEADER_MAGIC) << std::endl;
	sink << "LinkerVersion\t\t\t" << (int) _ioh.MajorLinkerVersion << "." << (int) _ioh.MinorLinkerVersion << std::endl;
	sink << "SizeOfCode\t\t\t" << _ioh.SizeOfCode << std::endl;
	sink << "SizeOfInitializedData\t\t" << _ioh.SizeOfInitializedData << std::endl;
	sink << "SizeOfUninitializedData\t\t" << _ioh.SizeOfUninitializedData << std::endl;
	sg::pimage_section_header sec = utils::find_section(_ioh.AddressOfEntryPoint, _section_table);
	if (sec != NULL) {
		sink << "AddressOfEntryPoint\t\t" << _ioh.AddressOfEntryPoint 
			 << " (Section: " << sec->Name << ")" << std::endl;
	}
	else {
		sink << "AddressOfEntryPoint\t\t" << _ioh.AddressOfEntryPoint 
			 << " (Section: ?)" << std::endl;
	}
	sink << "BaseOfCode\t\t\t" << _ioh.BaseOfCode << std::endl;

	// Field absent from PE32+ headers.
	if (_ioh.Magic == nt::IMAGE_OPTIONAL_HEADER_MAGIC["PE32"]) {
		sink << "BaseOfData\t\t\t" << _ioh.BaseOfData << std::endl;
	}

	sink << "ImageBase\t\t\t" << _ioh.ImageBase << std::endl;
	sink << "SectionAlignment\t\t" << _ioh.SectionAlignment << std::endl;
	sink << "FileAlignment\t\t\t" << _ioh.FileAlignment << std::endl;
	sink << "OperatingSystemVersion\t\t" << (int)_ioh.MajorOperatingSystemVersion << "." << (int)_ioh.MinorOperatingSystemVersion << std::endl;
	sink << "ImageVersion\t\t\t" << (int)_ioh.MajorImageVersion << "." << (int)_ioh.MinorImageVersion << std::endl;
	sink << "SubsystemVersion\t\t" << (int)_ioh.MajorSubsystemVersion << "." << (int)_ioh.MinorSubsystemVersion << std::endl;
	sink << "Win32VersionValue\t\t" << _ioh.Win32VersionValue << std::endl;
	sink << "SizeOfImage\t\t\t" << _ioh.SizeOfImage << std::endl;
	sink << "SizeOfHeaders\t\t\t" << _ioh.SizeOfHeaders << std::endl;
	sink << "Checksum\t\t\t" << _ioh.Checksum << std::endl;
	sink << "Subsystem\t\t\t" << nt::translate_to_flag(_ioh.Subsystem, nt::SUBSYSTEMS) << std::endl;
	sink << "DllCharacteristics\t\t";
	pretty_print_flags(sink, _ioh.DllCharacteristics, nt::DLL_CHARACTERISTICS, "\t\t\t\t");
	sink << "SizeofStackReserve\t\t" << _ioh.SizeofStackReserve << std::endl;
	sink << "SizeofStackCommit\t\t" << _ioh.SizeofStackCommit << std::endl;
	sink << "SizeofHeapReserve\t\t" << _ioh.SizeofHeapReserve << std::endl;
	sink << "SizeofHeapCommit\t\t" << _ioh.SizeofHeapCommit << std::endl;
	sink << "LoaderFlags\t\t\t" << _ioh.LoaderFlags << std::endl;
	sink << "NumberOfRvaAndSizes\t\t" << _ioh.NumberOfRvaAndSizes << std::endl << std::endl;
}

// ----------------------------------------------------------------------------

void PE::dump_section_table(std::ostream& sink) const
{
	if (!_initialized || _section_table.size() == 0) {
		return;
	}

	sink << "SECTION TABLE:" << std::endl << "--------------" << std::endl << std::endl;
	sink << std::hex;
	std::vector<std::string> flags;
	for (std::vector<pimage_section_header>::const_iterator it = _section_table.begin() ; it != _section_table.end() ; ++it)
	{
		sink << "Name\t\t\t" << (*it)->Name << std::endl;
		sink << "VirtualSize\t\t" << (*it)->VirtualSize << std::endl;
		sink << "VirtualAddress\t\t" << (*it)->VirtualAddress << std::endl;
		sink << "SizeOfRawData\t\t" << (*it)->SizeOfRawData << std::endl;
		sink << "PointerToRawData\t" << (*it)->PointerToRawData << std::endl;
		sink << "PointerToRelocations\t" << (*it)->PointerToRelocations << std::endl;
		sink << "PointerToLineNumbers\t" << (*it)->PointerToLineNumbers << std::endl;
		sink << "NumberOfRelocations\t" << (*it)->NumberOfRelocations << std::endl;
		sink << "NumberOfLineNumbers\t" << (*it)->NumberOfLineNumbers << std::endl;
		sink << "NumberOfRelocations\t" << (*it)->NumberOfRelocations << std::endl;
		sink << "Characteristics\t\t";
		flags = nt::translate_to_flags((*it)->Characteristics, nt::SECTION_CHARACTERISTICS);
		pretty_print_flags(sink, (*it)->Characteristics, nt::SECTION_CHARACTERISTICS);
		sink << std::endl;
	}
}

// ----------------------------------------------------------------------------

void PE::dump_imports(std::ostream& sink) const
{
	if (!_initialized || _imports.size() == 0) {
		return;
	}

	sink << "IMPORTS:" << std::endl << "--------" << std::endl << std::endl;
	std::vector<std::string> dlls = get_imported_dlls();
	for (std::vector<std::string>::iterator it = dlls.begin() ; it != dlls.end() ; ++it)
	{
		sink << *it << std::endl;
		std::vector<std::string> functions = get_imported_functions(*it);
		for (std::vector<std::string>::iterator it2 = functions.begin() ; it2 != functions.end() ; ++it2)
		{
			sink << "\t" << (*it2) << std::endl;
		}
		
	}

	sink << std::endl;
}

// ----------------------------------------------------------------------------

void PE::dump_exports(std::ostream& sink) const
{
	if (!_initialized || _exports.size() == 0) {
		return;
	}

	sink << "EXPORTS:" << std::endl << "--------" << std::endl << std::endl;
	for (std::vector<pexported_function>::const_iterator it = _exports.begin() ; it != _exports.end() ; ++it)
	{
		sink << std::dec << (*it)->Ordinal << "\t0x" << std::hex << (*it)->Address;
		if ((*it)->Name != "") {
			sink <<  "\t" << (*it)->Name;
		}
		if ((*it)->ForwardName != "") {
			sink <<  " -> " << (*it)->ForwardName;
		}
		sink << std::endl;
	}
	sink << std::endl;
}

// ----------------------------------------------------------------------------

void PE::dump_resources(std::ostream& sink) const
{
	if (!_initialized || _resource_table.size() == 0) {
		return;
	}

	sink << "RESOURCES:" << std::endl << "----------" << std::endl << std::endl;
	for (std::vector<pResource>::const_iterator it = _resource_table.begin() ; it != _resource_table.end() ; ++it)
	{
		sink << (*it)->get_name() << std::endl;
		sink << "\tType:\t\t" << (*it)->get_type() << std::endl;
		sink << "\tLanguage:\t" << (*it)->get_language() << std::endl;
		sink << "\tSize:\t\t0x" << std::hex << (*it)->get_size() << std::endl;
		yara::matches m = (*it)->detect_filetype();
		if (m.size() > 0) 
		{
			for (yara::matches::iterator it = m.begin() ; it != m.end() ; ++it) {
				sink <<	"\tDetected Type:\t" << (*it)->at("description") << std::endl;
			}
		}
	}
	sink << std::endl;
}

// ----------------------------------------------------------------------------

void PE::dump_version_info(std::ostream& sink) const
{
	if (!_initialized || _resource_table.size() == 0) {
		return;
	}

	pversion_info vi;
	for (std::vector<sg::pResource>::const_iterator it = _resource_table.begin() ; it != _resource_table.end() ; ++it)
	{
		if ((*it)->get_type() == "RT_VERSION")
		{
			vi = (*it)->interpret_as<sg::pversion_info>();
			if (vi == NULL) 
			{
				std::cerr << "Warning: Could not parse VERSION_INFO resource!" << std::endl;
				continue;
			}

			sink << "VERSION INFO:" << std::endl << "-------------" << std::endl << std::endl;
			sink << "Resource LangID:\t" << (*it)->get_language() << std::endl;
			sink << "Key:\t\t\t" << vi->Header.Key << std::endl;
			sink << "Value:" << std::endl;
			sink << "\tSignature:\t" << std::hex << vi->Value->Signature << std::endl;
			sink << "\tStructVersion:\t" << vi->Value->StructVersion << std::endl;
			sink << "\tFileVersion:\t" << utils::uint64_to_version_number(vi->Value->FileVersionMS, vi->Value->FileVersionLS) << std::endl;
			sink << "\tProductVersion:\t" << utils::uint64_to_version_number(vi->Value->ProductVersionMS, vi->Value->ProductVersionLS) << std::endl;
			sink << "\tFileFlags:\t";
			pretty_print_flags(sink, vi->Value->FileFlags & vi->Value->FileFlagsMask, nt::FIXEDFILEINFO_FILEFLAGS);
			sink << "\tFileOS:\t\t";
			pretty_print_flags(sink, vi->Value->FileOs, nt::FIXEDFILEINFO_FILEOS);
			sink << "\tFileType:\t" << nt::translate_to_flag(vi->Value->FileType, nt::FIXEDFILEINFO_FILETYPE) << std::endl;
			if (vi->Value->FileType == nt::FIXEDFILEINFO_FILETYPE["VFT_DRV"]) {
				sink << "\tFileSubtype\t"<< nt::translate_to_flag(vi->Value->FileSubtype, nt::FIXEDFILEINFO_FILESUBTYPE_DRV) << std::endl;
			}
			else if (vi->Value->FileType == nt::FIXEDFILEINFO_FILETYPE["VFT_FONT"]) {
				sink << "\tFileSubtype\t"<< nt::translate_to_flag(vi->Value->FileSubtype, nt::FIXEDFILEINFO_FILESUBTYPE_FONT) << std::endl;
			}
			sink << "\tLanguage:\t" << vi->Language << std::endl << std::endl;
			for (std::vector<ppair>::const_iterator it = vi->StringTable.begin() ; it != vi->StringTable.end() ; ++it) {
				sink << "\t" << (*it)->first << ": " << (*it)->second << std::endl;
			}
			sink << std::endl;
		}
	}
}

// ----------------------------------------------------------------------------

void PE::dump_debug_info(std::ostream& sink) const
{
	if (!_initialized || _debug_entries.size() == 0) {
		return;
	}

	sink << "DEBUG INFO:" << std::endl << "-----------" << std::endl;
	for (std::vector<pdebug_directory_entry>::const_iterator it = _debug_entries.begin() ; it != _debug_entries.end() ; ++it) 
	{
		sink << std::endl;
		sink << "\tCharacteristics:\t" << (*it)->Characteristics << std::endl;
		sink << "\tTimeDateStamp:\t\t" << (*it)->TimeDateStamp << std::endl;
		sink << "\tVersion:\t\t" << (*it)->MajorVersion << "." << (*it)->MinorVersion << std::endl;
		sink << "\tType:\t\t\t" << nt::translate_to_flag((*it)->Type, nt::DEBUG_TYPES) << std::endl;
		sink << "\tSizeofData:\t\t" << (*it)->SizeofData << std::endl;
		sink << "\tAddressOfRawData:\t" << (*it)->AddressOfRawData << std::endl;
		sink << "\tPointerToRawData:\t" << (*it)->PointerToRawData << std::endl;
		if ((*it)->Filename != "") {
			sink << "\tReferenced File:\t" << (*it)->Filename << std::endl;
		}
	}
	sink << std::endl;
}

// ----------------------------------------------------------------------------

void PE::dump_relocations(std::ostream& sink) const
{
	if (!_initialized || _relocations.size() == 0) {
		return;
	}

	sink << "RELOCATIONS:" << std::endl << "------------" << std::endl;
	for (std::vector<pimage_base_relocation>::const_iterator it = _relocations.begin() ; it != _relocations.end() ; ++it)
	{
		sink << std::endl;
		sink << "PageRVA:\t" << (*it)->PageRVA << std::endl;
		sink << "BlockSize:\t" << (*it)->BlockSize << std::endl;
		sink << "TypesOffsets:\t";
		for (std::vector<boost::uint16_t>::const_iterator it2 = (*it)->TypesOffsets.begin() ;  it2 != (*it)->TypesOffsets.end() ; ++it2)
		{
			if (it2 != (*it)->TypesOffsets.begin()) {
				sink << std::endl << "\t\t";
			}
			sink << nt::translate_to_flag((*it2 & 0xF000) >> 12, nt::BASE_RELOCATION_TYPES) << " - " << (*it2 & 0x0FFF);
		}
		sink << std::endl;
	}
	sink << std::endl;
}

// ----------------------------------------------------------------------------

void PE::dump_tls(std::ostream& sink) const
{
	if (!_initialized || _tls.Callbacks.size() == 0) {
		return;
	}

	int width = _ioh.Magic == nt::IMAGE_OPTIONAL_HEADER_MAGIC["PE32+"] ? 16 : 8;

	sink << "TLS Callbacks:" << std::endl << "--------------" << std::endl;
	sink << "StartAddressOfRawData:\t" << _tls.StartAddressOfRawData << std::endl;
	sink << "EndAddressOfRawData:\t" << _tls.EndAddressOfRawData << std::endl;
	sink << "AddressOfIndex:\t\t" << _tls.AddressOfIndex << std::endl;
	sink << "AddressOfCallbacks:\t" << _tls.AddressOfCallbacks << std::endl;
	sink << "SizeOfZeroFill:\t\t" << _tls.SizeOfZeroFill << std::endl;
	sink << "Characteristics:\t" << _tls.Characteristics << std::endl;
	sink << "Callbacks:\t\t" << std::hex;
	for (std::vector<boost::uint64_t>::const_iterator it = _tls.Callbacks.begin() ; it != _tls.Callbacks.end() ; ++it)
	{
		if (it == _tls.Callbacks.begin()) {
			sink << "0x" << *it << std::endl;
		}
		else {
			sink << "\t\t\t0x" << *it << std::endl;
		}
	}
	sink << std::endl;
}


} // !namespace sg