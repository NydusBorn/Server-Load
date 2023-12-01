package com.nydus.example.server_load_android

import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.Home
import androidx.compose.material.icons.filled.List
import androidx.compose.material.icons.filled.Search
import androidx.compose.material.icons.filled.Settings

object Constants {
    val BottomNavItems = listOf(
        navItem(label = "Priorities", icon = Icons.Default.List, route = "Priority"),
        navItem(label = "Buildings", icon = Icons.Default.Home, route = "Building"),
        navItem(label = "Research", icon = Icons.Default.Search, route = "Research"),
        navItem(label = "Settings", icon = Icons.Default.Settings, route = "Setting")
    )
    val BuildingItems = listOf(
        Building("Processes", 8, GameState.Processes),
        Building("Bits", 0, GameState.Building_Bits),
        Building("Bytes", 1, GameState.Building_Bytes),
        Building("Kilo packers", 2, GameState.Building_Kilo),
        Building("Mega packers", 3, GameState.Building_Mega),
        Building("Giga packers", 4, GameState.Building_Giga),
        Building("Tera packers", 5, GameState.Building_Tera),
        Building("Peta packers", 6, GameState.Building_Peta),
        Building("Exa packers", 7, GameState.Building_Exa)
    )
}