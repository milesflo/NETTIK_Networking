#pragma once

// Maximum name for a variable name. Increasing this will affect memory
// and performance on packet serialisation.
const size_t max_entvar_name = 64;

// Maxiumum data for variables. Increasing this will affect memory
// and performance on packet serailisation.
const size_t max_entvar_data = 128;
