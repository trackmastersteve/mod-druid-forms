-- Remove the old NPC if it exists to prevent duplicate key errors
DELETE FROM `creature_template` WHERE `entry` = 99999;
DELETE FROM `creature_template_model` WHERE `CreatureID` = 99999;

-- Insert the basic creature template with the new name
INSERT INTO `creature_template` (`entry`, `name`, `subname`, `gossip_menu_id`, `minlevel`, `maxlevel`, `faction`, `npcflag`, `speed_walk`, `speed_run`, `rank`, `unit_class`, `unit_flags`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `AIName`, `ScriptName`) VALUES
(99999, 'Keeper Remulos', 'Form Customizer', 0, 80, 80, 35, 1, 1, 1.14286, 0, 1, 0, 7, 0, 0, 0, 0, '', 'npc_druid_forms');

-- Assign the model ID (11906 = Keeper Remulos) 
INSERT INTO `creature_template_model` (`CreatureID`, `Idx`, `CreatureDisplayID`, `DisplayScale`, `Probability`, `VerifiedBuild`) VALUES 
(99999, 0, 11906, 0.4, 1.0, 0);
