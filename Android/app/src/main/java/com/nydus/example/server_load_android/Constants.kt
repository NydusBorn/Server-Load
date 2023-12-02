package com.nydus.example.server_load_android

import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.Home
import androidx.compose.material.icons.filled.List
import androidx.compose.material.icons.filled.Search
import androidx.compose.material.icons.filled.Settings
import androidx.compose.runtime.MutableFloatState
import androidx.compose.runtime.mutableFloatStateOf
import androidx.core.content.res.FontResourcesParserCompat.FamilyResourceEntry

object Constants {
    val BottomNavItems = listOf(
        navItem(label = "Priorities", icon = Icons.Default.List, route = "Priority"),
        navItem(label = "Buildings", icon = Icons.Default.Home, route = "Building"),
        navItem(label = "Research", icon = Icons.Default.Search, route = "Research"),
        navItem(label = "Settings", icon = Icons.Default.Settings, route = "Setting")
    )
    val BuildingItems = listOf(
        Building("Processes", 8, GameState.Processes, 4, 0f, mutableFloatStateOf(0f)),
        Building("Bits", 0, GameState.Building_Bits, 0, 0f, mutableFloatStateOf(0f)),
        Building("Bytes", 1, GameState.Building_Bytes, 1, 80f, GameState.Building_Bits),
        Building("Kilo packers", 2, GameState.Building_Kilo, 2, 10240f, GameState.Building_Bytes),
        Building("Mega packers", 3, GameState.Building_Mega, 3, 10240f, GameState.Building_Kilo),
        Building("Giga packers", 4, GameState.Building_Giga, 4, 10240f, GameState.Building_Mega),
        Building("Tera packers", 5, GameState.Building_Tera, 5, 10240f, GameState.Building_Giga),
        Building("Peta packers", 6, GameState.Building_Peta, 6, 10240f, GameState.Building_Tera),
        Building("Exa packers", 7, GameState.Building_Exa, 7, 10240f, GameState.Building_Peta),
    )
    val ResearchItems = listOf(
        Research("Process Multiplier", 16, GameState.Research_Process_Mul, 5, 0f, mutableFloatStateOf(0f)),
        Research("Bits Add", 0, GameState.Research_Bits_Add, 0, 4f, GameState.Building_Bits),
        Research("Bits Multiplier", 1, GameState.Research_Bits_Mul, 1, 8f, GameState.Research_Bits_Add),
        Research("Bytes Add", 2, GameState.Research_Bytes_Add, 1, 512f, GameState.Building_Bytes),
        Research("Bytes Multiplier", 3, GameState.Research_Bytes_Mul, 2, 8f, GameState.Research_Bytes_Add),
        Research("Kilo packers Add", 4, GameState.Research_Kilo_Add, 2, 512f, GameState.Building_Kilo),
        Research("Kilo packers Multiplier", 5, GameState.Research_Kilo_Mul, 3, 8f, GameState.Research_Kilo_Add),
        Research("Mega packers Add", 6, GameState.Research_Mega_Add, 3, 512f, GameState.Building_Mega),
        Research("Mega packers Multiplier", 7, GameState.Research_Mega_Mul, 4, 8f, GameState.Research_Mega_Add),
        Research("Giga packers Add", 8, GameState.Research_Giga_Add, 4, 512f, GameState.Building_Giga),
        Research("Giga packers Multiplier", 9, GameState.Research_Giga_Mul, 5, 8f, GameState.Research_Giga_Add),
        Research("Tera packers Add", 10, GameState.Research_Tera_Add, 5, 512f, GameState.Building_Tera),
        Research("Tera packers Multiplier", 11, GameState.Research_Tera_Mul, 6, 8f, GameState.Research_Tera_Add),
        Research("Peta packers Add", 12, GameState.Research_Peta_Add, 6, 512f, GameState.Building_Peta),
        Research("Peta packers Multiplier", 13, GameState.Research_Peta_Mul, 7, 8f, GameState.Research_Peta_Add),
        Research("Exa packers Add", 14, GameState.Research_Exa_Add, 7, 512f, GameState.Building_Exa),
        Research("Exa packers Multiplier", 15, GameState.Research_Exa_Mul, 7, 8f, GameState.Research_Exa_Add),
    )
}