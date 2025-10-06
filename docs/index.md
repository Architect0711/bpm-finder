---
layout: default
title: Home
---

# bpm-finder

A C++ application for BPM detection.

## Documentation

### Core

- [Sink](core/sink.md) - Base class for DSP chain consumers

### App

- [Overview](app/overview.md) - Application architecture and structure

## Recent Posts

{% for post in site.posts limit:5 %}

- [{{ post.title }}]({{ post.url }}) - {{ post.date | date: "%B %d, %Y" }}
  {% endfor %}
