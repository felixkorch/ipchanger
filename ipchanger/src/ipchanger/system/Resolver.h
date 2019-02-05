#pragma once
#ifndef RESOLVER_H
#define RESOLVER_H

#include <optional>

namespace ipchanger::system {

	std::optional<std::string> Resolve(const std::string& hostname);

} // namespace

#endif // RESOLVER_H
