CREATE TABLE IF NOT EXISTS `character_druid_forms` (
  `guid` INT UNSIGNED NOT NULL COMMENT 'Player GUID',
  `form_id` TINYINT UNSIGNED NOT NULL COMMENT 'Shapeshift Form ID (e.g. 1=Cat, 5=Bear)',
  `model_id` INT UNSIGNED NOT NULL COMMENT 'Custom Display ID',
  PRIMARY KEY (`guid`, `form_id`)
);
