-- Remove the old NPC if it exists to prevent duplicate key errors
DELETE FROM `creature_template` WHERE `entry` = 99999;
DELETE FROM `creature_template_model` WHERE `CreatureID` = 99999;

-- Insert the basic creature template without modelid1 and without scale
INSERT INTO `creature_template` (`entry`, `name`, `subname`, `gossip_menu_id`, `minlevel`, `maxlevel`, `faction`, `npcflag`, `speed_walk`, `speed_run`, `rank`, `unit_class`, `unit_flags`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `AIName`, `ScriptName`) VALUES
(99999, 'Broll Bearmantle', 'Form Customizer', 0, 80, 80, 35, 1, 1, 1.14286, 0, 1, 0, 7, 0, 0, 0, 0, '', 'npc_druid_forms');

-- Assign the model ID (17351 = Broll Night Elf) and scale (1.0) using the modern table structure
INSERT INTO `creature_template_model` (`CreatureID`, `Idx`, `CreatureDisplayID`, `DisplayScale`, `Probability`, `VerifiedBuild`) VALUES 
(99999, 0, 17342, 1.0, 1.0, 0);
