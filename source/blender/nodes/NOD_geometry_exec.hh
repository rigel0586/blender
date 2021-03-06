/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#pragma once

#include "FN_generic_value_map.hh"

#include "BKE_attribute_access.hh"
#include "BKE_geometry_set.hh"
#include "BKE_persistent_data_handle.hh"

#include "DNA_node_types.h"

namespace blender::nodes {

using bke::Color4fReadAttribute;
using bke::Color4fWriteAttribute;
using bke::Float3ReadAttribute;
using bke::Float3WriteAttribute;
using bke::FloatReadAttribute;
using bke::FloatWriteAttribute;
using bke::PersistentDataHandleMap;
using bke::PersistentObjectHandle;
using bke::ReadAttribute;
using bke::ReadAttributePtr;
using bke::WriteAttribute;
using bke::WriteAttributePtr;
using fn::CPPType;
using fn::GMutablePointer;
using fn::GValueMap;

class GeoNodeExecParams {
 private:
  const bNode &node_;
  GValueMap<StringRef> &input_values_;
  GValueMap<StringRef> &output_values_;
  const PersistentDataHandleMap &handle_map_;
  const Object *self_object_;

 public:
  GeoNodeExecParams(const bNode &node,
                    GValueMap<StringRef> &input_values,
                    GValueMap<StringRef> &output_values,
                    const PersistentDataHandleMap &handle_map,
                    const Object *self_object)
      : node_(node),
        input_values_(input_values),
        output_values_(output_values),
        handle_map_(handle_map),
        self_object_(self_object)
  {
  }

  /**
   * Get the input value for the input socket with the given identifier.
   *
   * The node calling becomes responsible for destructing the value before it is done
   * executing. This method can only be called once for each identifier.
   */
  GMutablePointer extract_input(StringRef identifier)
  {
#ifdef DEBUG
    this->check_extract_input(identifier);
#endif
    return input_values_.extract(identifier);
  }

  /**
   * Get the input value for the input socket with the given identifier.
   *
   * This method can only be called once for each identifier.
   */
  template<typename T> T extract_input(StringRef identifier)
  {
#ifdef DEBUG
    this->check_extract_input(identifier, &CPPType::get<T>());
#endif
    return input_values_.extract<T>(identifier);
  }

  /**
   * Get the input value for the input socket with the given identifier.
   *
   * This makes a copy of the value, which is fine for most types but should be avoided for
   * geometry sets.
   */
  template<typename T> T get_input(StringRef identifier) const
  {
#ifdef DEBUG
    this->check_extract_input(identifier, &CPPType::get<T>());
#endif
    return input_values_.lookup<T>(identifier);
  }

  /**
   * Move-construct a new value based on the given value and store it for the given socket
   * identifier.
   */
  void set_output_by_move(StringRef identifier, GMutablePointer value)
  {
#ifdef DEBUG
    BLI_assert(value.type() != nullptr);
    BLI_assert(value.get() != nullptr);
    this->check_set_output(identifier, *value.type());
#endif
    output_values_.add_new_by_move(identifier, value);
  }

  /**
   * Store the output value for the given socket identifier.
   */
  template<typename T> void set_output(StringRef identifier, T &&value)
  {
#ifdef DEBUG
    this->check_set_output(identifier, CPPType::get<std::decay_t<T>>());
#endif
    output_values_.add_new(identifier, std::forward<T>(value));
  }

  /**
   * Get the node that is currently being executed.
   */
  const bNode &node() const
  {
    return node_;
  }

  const PersistentDataHandleMap &handle_map() const
  {
    return handle_map_;
  }

  const Object *self_object() const
  {
    return self_object_;
  }

  /**
   * Creates a read-only attribute based on node inputs. The method automatically detects which
   * input with the given name is available.
   */
  ReadAttributePtr get_input_attribute(const StringRef name,
                                       const GeometryComponent &component,
                                       const AttributeDomain domain,
                                       const CustomDataType type,
                                       const void *default_value) const;

  template<typename T>
  bke::TypedReadAttribute<T> get_input_attribute(const StringRef name,
                                                 const GeometryComponent &component,
                                                 const AttributeDomain domain,
                                                 const T &default_value) const
  {
    const CustomDataType type = bke::cpp_type_to_custom_data_type(CPPType::get<T>());
    return this->get_input_attribute(name, component, domain, type, &default_value);
  }

  /**
   * Get the type of an input property or the associated constant socket types with the
   * same names. Fall back to the default value if no attribute exists with the name.
   */
  CustomDataType get_input_attribute_data_type(const StringRef name,
                                               const GeometryComponent &component,
                                               const CustomDataType default_type) const;

 private:
  /* Utilities for detecting common errors at when using this class. */
  void check_extract_input(StringRef identifier, const CPPType *requested_type = nullptr) const;
  void check_set_output(StringRef identifier, const CPPType &value_type) const;

  /* Find the active socket socket with the input name (not the identifier). */
  const bNodeSocket *find_available_socket(const StringRef name) const;
};

}  // namespace blender::nodes
