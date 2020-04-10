create database covid19;

create table notifications (
  id serial primary key,
  entity varchar(64) not null,
  code char(16) not null,
  notification_date date not null,
  cases int not null default 0,
  deaths int not null default 0,
  unique (code, notification_date)
);

create table samples (
  sample_id integer not null,
  seq integer not null,
  value integer not null,
  type char(1) not null check (type in ('T', 'V', 'X')),
  primary key (sample_id, seq)
);
